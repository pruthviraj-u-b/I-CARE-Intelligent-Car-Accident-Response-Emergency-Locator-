#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <math.h>

// ---------- Pins ----------
#define LCD_SDA_PIN   8
#define LCD_SCL_PIN   9
#define IMU_SDA_PIN   2
#define IMU_SCL_PIN   3
#define BUZZER_PIN    20
#define BUTTON_PIN    15
#define MQ2_PIN       16
#define TRIGGER_PIN   14

LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_MPU6050 mpu;

// ---------- GPS ----------
SoftwareSerial gpsSS(4, 6);
TinyGPSPlus gps;

// ---------- GSM ----------
#define EMERGENCY1 "+917022880385"
#define EMERGENCY2 "+919108735137"
#define EMERGENCY3 "+919740166569"

// ---------- SETTINGS ----------
#define CRASH_THRESHOLD_G 1.0
#define ALERT_COOLDOWN 15000

unsigned long lastAlertTime = 0;
bool lastDrowsyState = LOW;

// ---------- LCD ----------
void lcdPrintLine(uint8_t row, const char* s) {
  lcd.setCursor(0, row);
  lcd.print("                ");
  lcd.setCursor(0, row);
  lcd.print(s);
}

// ---------- BUZZER ----------
void beep() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(700);
  digitalWrite(BUZZER_PIN, LOW);
}

// ---------- GPS ----------
void updateGPS() {
  while (gpsSS.available()) gps.encode(gpsSS.read());
}

// ---------- MESSAGE ----------
String buildMessage(const char* type, float peak) {

  String msg = "EMERGENCY DETECTED !\r\n";

  msg += "Type: ";
  msg += type;
  msg += "\r\n";

  if (peak > 0) {
    msg += "Impact: ";
    msg += String(peak, 2);
    msg += "g\r\n";
  }

  // ✅ LIVE GPS
  if (gps.location.isValid() && gps.location.age() < 2000 &&
      gps.location.lat() != 0.0 && gps.location.lng() != 0.0) {

    double lat = gps.location.lat();
    double lon = gps.location.lng();

    msg += "Lat: ";
    msg += String(lat, 6);
    msg += "  Lon: ";
    msg += String(lon, 6);
    msg += " (live*)\r\n";

    msg += "https://maps.google.com/?q=";
    msg += String(lat, 6);
    msg += ",";
    msg += String(lon, 6);
    msg += " (live*)";
  }
  else {
    msg += "GPS : NO FIX";
  }

  return msg;
}

// ---------- CALL ----------
void makeCall(const char* number) {
  lcdPrintLine(0, "CALLING...");
  lcdPrintLine(1, number);

  Serial1.print("ATD");
  Serial1.print(number);
  Serial1.println(";");

  delay(8000);

  Serial1.println("ATH");
  delay(1000);
}

// ---------- SMS ----------
void sendSMS(const char* number, String msg) {
  Serial1.print("AT+CMGS=\"");
  Serial1.print(number);
  Serial1.println("\"");

  delay(300);

  Serial1.print(msg);
  Serial1.write(26);

  delay(2000);
}

// ---------- EMERGENCY ----------
void triggerEmergency(const char* type, float peak) {

  if (millis() - lastAlertTime < ALERT_COOLDOWN) return;
  lastAlertTime = millis();

  lcd.clear();
  lcdPrintLine(0, "*** ALERT ***");
  lcdPrintLine(1, type);

  beep();

  String msg = buildMessage(type, peak);

  Serial1.println("AT");
  delay(100);
  Serial1.println("AT+CMGF=1");
  delay(100);

  makeCall(EMERGENCY1);

  sendSMS(EMERGENCY1, msg);
  sendSMS(EMERGENCY2, msg);
  sendSMS(EMERGENCY3, msg);

  lcdPrintLine(0, "SOS SENT!");
  lcdPrintLine(1, type);
  delay(1500);
}

// ---------- SETUP ----------
void setup() {

  Wire.setSDA(LCD_SDA_PIN);
  Wire.setSCL(LCD_SCL_PIN);
  Wire.begin();

  lcd.init();
  lcd.backlight();

  lcdPrintLine(0, "System Ready");
  lcdPrintLine(1, "Initializing...");
  delay(2000);
  lcd.clear();

  Wire1.setSDA(IMU_SDA_PIN);
  Wire1.setSCL(IMU_SCL_PIN);
  Wire1.begin();

  if (!mpu.begin(MPU6050_I2CADDR_DEFAULT, &Wire1)) {
    lcdPrintLine(0, "MPU ERROR!");
    while (1);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
  pinMode(MQ2_PIN, INPUT);
  pinMode(TRIGGER_PIN, INPUT);

  Serial1.setTX(0);
  Serial1.setRX(1);
  Serial1.begin(115200);

  gpsSS.begin(9600);
  gpsSS.listen();
}

// ---------- LOOP ----------
void loop() {

  updateGPS();

  // ---- CRASH ----
  sensors_event_t a, g, t;
  mpu.getEvent(&a, &g, &t);

  float mag = sqrt(a.acceleration.x * a.acceleration.x +
                   a.acceleration.y * a.acceleration.y +
                   a.acceleration.z * a.acceleration.z);

  float crash_g = fabs((mag / 9.80665) - 1.0);

  char l0[17], l1[17];
  snprintf(l0, sizeof(l0), "G:%5.2f g", crash_g);
  snprintf(l1, sizeof(l1), "Thresh:%4.1f", CRASH_THRESHOLD_G);

  lcdPrintLine(0, l0);
  lcdPrintLine(1, l1);

  if (crash_g >= CRASH_THRESHOLD_G) {
    triggerEmergency("CRASH", crash_g);
  }

  // ---- SOS ----
  if (digitalRead(BUTTON_PIN) == HIGH) {
    triggerEmergency("SOS", 0);
  }

  // ---- GAS ----
  if (digitalRead(MQ2_PIN) == LOW) {
    triggerEmergency("GAS DETECTED", 0);
  }

  // ---- DROWSINESS ----
  bool currentDrowsyState = digitalRead(TRIGGER_PIN);
  if (currentDrowsyState == HIGH && lastDrowsyState == LOW) {
    delay(50);
    if (digitalRead(TRIGGER_PIN) == HIGH) {
      triggerEmergency("DROWSINESS", 0);
    }
  }
  lastDrowsyState = currentDrowsyState;

  delay(100);
}