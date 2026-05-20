# I-CARE-Intelligent-Car-Accident-Response-Emergency-Locator
IoT &amp; Computer Vision framework for proactive vehicle accident prevention &amp; autonomous emergency response. Built with Raspberry Pi 4B &amp; Pico, it uses OpenCV for driver drowsiness detection (EAR) and sensor fusion (MPU6050/MQ2) to detect crashes/fires, instantly broadcasting GPS coordinates via GSM within 5 seconds.

# I-CARE: Intelligent Car Accident Response Emergency Locator

I-CARE is an automated, life-saving IoT and Computer Vision framework designed to eliminate delays in emergency medical intervention during the critical "Golden Hour". It integrates proactive driver fatigue monitoring with autonomous accident detection and rapid location reporting.

## 🧠 System Architecture & Key Features
- **Dual-Processor Core:** Leverages a Raspberry Pi 4B for high-level computer vision processing and a Raspberry Pi Pico for low-latency, real-time sensor polling.
- **Proactive Fatigue Mitigation:** Utilizes a Pi Camera and OpenCV to calculate Eye Aspect Ratio (EAR) for real-time driver drowsiness detection.
- **Multi-Hazard Sensor Fusion:** Uses an MPU6050 (6-axis IMU) for high-impact collision/rollover sensing and an MQ2 sensor for internal cabin fire/smoke detection.
- **Autonomous Rescue Link:** Instantly captures coordinates via a NEO-6M GPS module and triggers a GSM module to initiate an emergency voice call and broadcast an SMS with a clickable Google Maps link.
- **Driver Override Feature:** Features a 30-second buzzer-activated window allowing drivers to manually cancel false alarms.

## 📈 Validated Performance Results
- **95.5% average success rate** across all crash, smoke, and driver fatigue detection modules.
- **100% accuracy** achieved on manual SOS triggers.
- **Ultra-low latency connection:** Emergency voice calls connect in 3 seconds, and location-tracked SMS alerts deliver within 5 seconds under typical conditions.

## 🛠️ Tech Stack & Hardware Components
- **Languages/Libraries:** Python, OpenCV
- **Hardware:** Raspberry Pi 4B, Raspberry Pi Pico, MPU6050 Accelerometer/Gyroscope, MQ2 Smoke/Gas Sensor, NEO-6M GPS Module, SIM800L/A7670E GSM Module, Pi Camera, Buzzer.
