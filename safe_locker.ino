
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <Preferences.h>

// ---------------- OLED DISPLAY ----------------
U8G2_SSD1306_128X64_NONAME_F_HW_I2C oled(
  U8G2_R0, U8X8_PIN_NONE
);

Preferences preferences;

// ---------------- PIN CONFIGURATION ----------------
// For a classic ESP32 DevKit
const int POT_PIN = 34;
const int GREEN_LED = 25;
const int RED_LED = 26;
const int BUTTON_PIN = 0;  // BOOT button, active LOW

// OLED I2C: SDA = GPIO 21, SCL = GPIO 22

// ---------------- SECURITY SETTINGS ----------------
const int CODE_LENGTH = 4;
const int MAX_ATTEMPTS = 3;

const unsigned long LOCKOUT_TIME = 30000UL;
const unsigned long DEBOUNCE_TIME = 40;
const unsigned long LONG_PRESS_TIME = 2000;

// Default passcode on first use
const uint32_t DEFAULT_CODE = 2580;

uint32_t savedCode = DEFAULT_CODE;
int enteredDigits[CODE_LENGTH];
int digitCount = 0;
int failedAttempts = 0;

enum Mode {
  IDLE,
  ENTER_CODE,
  SET_CODE,
  LOCKOUT
};

Mode mode = IDLE;

unsigned long lockoutStart = 0;
unsigned long buttonDownTime = 0;
unsigned long lastButtonChange = 0;

bool lastRawButton = HIGH;
bool stableButton = HIGH;
bool longPressHandled = false;

unsigned long messageUntil = 0;
String statusMessage = "Ready";

// ---------------- POTENTIOMETER ----------------
int getSelectedDigit() {
  int value = analogRead(POT_PIN);
  return map(value, 0, 4095, 0, 9);
}

// ---------------- DISPLAY ----------------
void showScreen() {
  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);

  if (mode == LOCKOUT) {
    unsigned long elapsed = millis() - lockoutStart;
    int remaining = (elapsed >= LOCKOUT_TIME)
                      ? 0
                      : (LOCKOUT_TIME - elapsed + 999) / 1000;

    oled.drawStr(0, 12, "SECURITY LOCKOUT");
    oled.drawStr(0, 30, "Too many failures");
    oled.drawStr(0, 46, "Try again in:");
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%d seconds", remaining);
    oled.drawStr(0, 62, buffer);
  }
  else if (mode == SET_CODE) {
    oled.drawStr(0, 12, "SET NEW PASSCODE");
    oled.drawStr(0, 29, "Select digit:");
    char buffer[24];
    snprintf(buffer, sizeof(buffer), "Digit: %d",
             getSelectedDigit());
    oled.drawStr(0, 43, buffer);

    snprintf(buffer, sizeof(buffer), "Confirmed: %d/%d",
             digitCount, CODE_LENGTH);
    oled.drawStr(0, 60, buffer);
  }
  else if (mode == ENTER_CODE) {
    oled.drawStr(0, 12, "ENTER PASSCODE");
    oled.drawStr(0, 29, "Select digit:");
    char buffer[24];
    snprintf(buffer, sizeof(buffer), "Digit: %d",
             getSelectedDigit());
    oled.drawStr(0, 43, buffer);

    oled.drawStr(0, 60, "Confirmed: ");
    for (int i = 0; i < digitCount; i++) {
      oled.drawStr(69 + (i * 10), 60, "*");
    }
  }
  else {
    oled.drawStr(0, 12, "SMART SAFE LOCKER");
    oled.drawStr(0, 30, statusMessage.c_str());

    if (millis() < messageUntil) {
      oled.drawStr(0, 48, "Press to continue");
    } else {
      oled.drawStr(0, 47, "Short: Enter code");
      oled.drawStr(0, 62, "Hold 2s: Set code");
    }
  }

  oled.sendBuffer();
}

// ---------------- LED CONTROL ----------------
void ledsOff() {
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
}

void successSignal() {
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);
}

void errorSignal() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, LOW);
}

// ---------------- RESET INPUT ----------------
void clearEnteredCode() {
  digitCount = 0;
  for (int i = 0; i < CODE_LENGTH; i++) {
    enteredDigits[i] = 0;
  }
}

// ---------------- PASSCODE COMPARISON ----------------
uint32_t digitsToNumber() {
  uint32_t result = 0;

  for (int i = 0; i < CODE_LENGTH; i++) {
    result = result * 10 + enteredDigits[i];
  }

  return result;
}

void startLockout() {
  mode = LOCKOUT;
  lockoutStart = millis();
  statusMessage = "Locked for 30 sec";
  ledsOff();
  Serial.println("LOCKOUT: 30 seconds");
}

void validateCode() {
  uint32_t candidate = digitsToNumber();

  if (candidate == savedCode) {
    failedAttempts = 0;
    mode = IDLE;
    statusMessage = "ACCESS GRANTED";
    messageUntil = millis() + 2500;
    successSignal();

    Serial.println("Access granted");
  } else {
    failedAttempts++;
    errorSignal();

    Serial.printf("Incorrect code. Attempts: %d/%d\n",
                  failedAttempts, MAX_ATTEMPTS);

    clearEnteredCode();

    if (failedAttempts >= MAX_ATTEMPTS) {
      startLockout();
    } else {
      mode = IDLE;
      statusMessage = "Incorrect passcode";
      messageUntil = millis() + 2000;
    }
  }
}

// ---------------- PASSCODE SETUP ----------------
void saveNewCode() {
  savedCode = digitsToNumber();

  preferences.putUInt("safeCode", savedCode);

  mode = IDLE;
  failedAttempts = 0;
  statusMessage = "Passcode saved";
  messageUntil = millis() + 2500;

  clearEnteredCode();
  ledsOff();

  Serial.println("New passcode saved");
}

// ---------------- BUTTON ACTION ----------------
void handleShortPress() {
  if (mode == LOCKOUT) {
    return;
  }

  if (mode == IDLE) {
    mode = ENTER_CODE;
    clearEnteredCode();
    statusMessage = "Enter passcode";
    messageUntil = 0;
  }

  if (mode == ENTER_CODE || mode == SET_CODE) {
    enteredDigits[digitCount] = getSelectedDigit();
    digitCount++;

    if (digitCount >= CODE_LENGTH) {
      if (mode == SET_CODE) {
        saveNewCode();
      } else {
        validateCode();
      }
    }
  }
}

void handleLongPress() {
  if (mode != IDLE) {
    return;
  }

  mode = SET_CODE;
  clearEnteredCode();
  ledsOff();

  statusMessage = "Set new passcode";
  messageUntil = 0;

  Serial.println("Passcode setup mode");
}

// ---------------- BUTTON DEBOUNCE ----------------
void updateButton() {
  bool rawButton = digitalRead(BUTTON_PIN);

  if (rawButton != lastRawButton) {
    lastButtonChange = millis();
    lastRawButton = rawButton;
  }

  if (millis() - lastButtonChange >= DEBOUNCE_TIME) {
    if (rawButton != stableButton) {
      stableButton = rawButton;

      if (stableButton == LOW) {
        buttonDownTime = millis();
        longPressHandled = false;
      } else {
        if (!longPressHandled) {
          handleShortPress();
        }
      }
    }
  }

  if (stableButton == LOW &&
      !longPressHandled &&
      millis() - buttonDownTime >= LONG_PRESS_TIME) {

    if (mode == IDLE) {
      handleLongPress();
      longPressHandled = true;
    }
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(115200);

  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  analogReadResolution(12);
  ledsOff();

  Wire.begin(21, 22);
  oled.begin();

  preferences.begin("safeLocker", false);

  savedCode = preferences.getUInt("safeCode", DEFAULT_CODE);

  oled.clearBuffer();
  oled.setFont(u8g2_font_6x12_tf);
  oled.drawStr(0, 20, "SMART SAFE LOCKER");
  oled.drawStr(0, 42, "System Starting...");
  oled.sendBuffer();

  delay(1200);

  statusMessage = "Ready";
  showScreen();

  Serial.println("Smart Safe Locker Ready");
  Serial.println("Short press: enter passcode");
  Serial.println("Hold BOOT 2 sec: set new passcode");
}

// ---------------- MAIN LOOP ----------------
void loop() {
  updateButton();

  if (mode == LOCKOUT &&
      millis() - lockoutStart >= LOCKOUT_TIME) {

    mode = IDLE;
    failedAttempts = 0;
    clearEnteredCode();

    statusMessage = "Lockout complete";
    messageUntil = millis() + 2000;

    ledsOff();

    Serial.println("Lockout ended");
  }

  static unsigned long lastDisplayUpdate = 0;

  if (millis() - lastDisplayUpdate >= 100) {
    lastDisplayUpdate = millis();
    showScreen();
  }
}
