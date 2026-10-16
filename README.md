# ESP32-Based Smart Safe Locker with OLED Display and Timed Security Lockout

## 📌 Project Overview

The ESP32-Based Smart Safe Locker is an embedded security system designed to provide controlled access using a configurable passcode. It uses an ESP32 microcontroller, a potentiometer to set passcode values, an OLED display for real-time status updates, and red and green LEDs for visual feedback. After three consecutive incorrect attempts, the system activates a 30-second security lockout to prevent further access attempts.

## ✨ Features

- **ESP32-Based Control:** Handles passcode validation and security logic.
- **Configurable Passcode:** Uses a potentiometer to set passcode values.
- **OLED Display:** Displays system status, passcode entry feedback, and lockout information.
- **LED Indicators:** Green LED indicates successful access, while the red LED indicates incorrect attempts.
- **Timed Security Lockout:** Blocks further attempts for 30 seconds after three incorrect entries.
- **Real-Time Feedback:** Provides immediate visual updates during user interaction.

## 🛠️ Hardware Components

- ESP32 development board
- OLED display
- Potentiometer
- Red LED
- Green LED
- Breadboard
- Jumper wires
- USB cable and suitable power supply

## 💻 Software and Technologies

- Arduino IDE
- Embedded C/C++ 
- ESP32 microcontroller programming
- I2C communication for the OLED display, if applicable

## ⚙️ Working Principle

1. **System Initialization:** The ESP32 initializes the OLED display and LED indicators.
2. **Passcode Configuration:** The potentiometer is used to set the passcode values through the implemented input mechanism.
3. **Passcode Validation:** The ESP32 checks the entered passcode against the configured value.
4. **Successful Access:** The green LED turns on and the OLED display indicates successful access.
5. **Incorrect Attempt:** The red LED indicates an incorrect passcode, and the failed-attempt counter increases.
6. **Security Lockout:** After three consecutive incorrect attempts, the system blocks further attempts for 30 seconds.
7. **Lockout Reset:** Once the timer expires, the system allows new attempts.

## 🔌 Circuit Connections

Add your actual circuit diagram and pin connections here.

| Component | ESP32 Connection |
|---|---|
| OLED Display | I2C SDA and SCL pins |
| Potentiometer | Configured analog input and appropriate supply connections |
| Red LED | Configured digital output through a current-limiting resistor |
| Green LED | Configured digital output through a current-limiting resistor |
| Common Ground | GND |

*Update the connections according to your actual circuit and source code.*

## 📂 Project Structure

```text
esp32-smart-safe-locker/
├── README.md
├── src/
│   └── safe_locker.ino
├── images/
│   ├── prototype.jpg
│   └── circuit_diagram.png
└── docs/
    └── project_report.pdf
```

*Add the folders and files that you actually have.*

## 🚀 How to Run

1. Install the Arduino IDE.
2. Install the ESP32 board package.
3. Install the required OLED display library.
4. Connect the components according to your circuit diagram.
5. Open the Arduino source code.
6. Select the appropriate ESP32 board and COM port.
7. Upload the program to the ESP32.
8. Test successful access, incorrect attempts, and the 30-second lockout.

## 🎯 Applications

- Educational embedded-systems projects
- Basic electronic access-control prototypes
- Laboratory equipment security concepts
- Smart storage and locker systems
- Microcontroller-based security demonstrations

## 🔮 Future Improvements

- Add a servo motor or solenoid lock for physical locking.
- Store passcode settings in non-volatile memory.
- Add a buzzer for audible alerts.
- Introduce tamper detection and access logs.
- Integrate Wi-Fi notifications for remote monitoring.
- Improve the enclosure and power management.

## 📸 Project Demonstration

Add photographs or a demonstration video of your working prototype here.

Include images showing:
- The complete hardware setup
- Passcode configuration
- Successful access
- Incorrect passcode feedback
- The 30-second lockout countdown

## 👨‍💻 Author

**Vishnu Varthan VG**

Electrical and Electronics Engineering (EEE)

GitHub: [Add your GitHub profile link]

LinkedIn: [Add your LinkedIn profile link]

## 📄 License

This project is intended for educational and demonstration purposes. Add a suitable open-source license if you want others to reuse or modify your work.
