#include "lift.h"
#include <ESP32Servo.h>
#include "aerolitheUdp.h"

Servo ttservo;

// Constants
const int pwmPin = 14;  // ADC-capable pin
const int limitSwitchPin = 22;
const int minPWM = 500;
const int maxPWM = 2500;
const int turnsToMicroseconds = 1000;  // Adjust this value based on your servo's specifications
const int maxTurns = 5;
const int maxPosition = minPWM + (maxTurns * turnsToMicroseconds);

// Define variables for the limit switch flags
volatile bool limitSwitchLiftInterruptTriggered = false;
volatile bool limitSwitchLiftTriggered = false;

const int debounceDelay = 100;  // Debounce delay in milliseconds
unsigned long limitSwitchLastDebounceTime = 0;

int currentServoPosition = 0;  // Global variable to track the servo position

void IRAM_ATTR onLiftLimitSwitchTriggered() {
  limitSwitchLiftInterruptTriggered = true;
}

void liftServoSetup() {
  ttservo.attach(pwmPin, minPWM, maxPWM);
  ttservo.setPeriodHertz(60);
  delay(100);  // Add a delay to allow the servo to initialize
  pinMode(limitSwitchPin, INPUT_PULLUP);
  // Attach interrupts to the limit switch pins
  attachInterrupt(digitalPinToInterrupt(limitSwitchPin), onLiftLimitSwitchTriggered, FALLING);

  // Move to the limit switch at power-up
  liftServoSetZero();
}

void liftServoSetZero() {
  Serial.println("Performing a Lift Servo Calibration");
  limitSwitchLiftTriggered = false;
  ttservo.attach(pwmPin, minPWM, maxPWM);

  // Check if the limit switch is already triggered
  if (digitalRead(limitSwitchPin) == LOW) {
    Serial.println("Limit switch already triggered at power-up");
    currentServoPosition = minPWM;  // Set the current position to zero
    return;
  }

  ttservo.writeMicroseconds(minPWM);  // Move to the minimum position
  delay(100);                         // Allow some time for the servo to start moving

  unsigned long startTime = millis();
  while (digitalRead(limitSwitchPin) == HIGH) {
    // Wait until the limit switch is triggered or 10 seconds have passed
    if (millis() - startTime > 10000) {  // 10 seconds timeout
      Serial.println("Timeout: Limit switch not triggered within 10 seconds");
      currentServoPosition = minPWM;  // Set the current position to zero
      return;
    }
  }
  currentServoPosition = minPWM;  // Set the current position to zero
}

void liftPerformSetPosition(const int microseconds) {
  if (microseconds >= minPWM && microseconds <= maxPosition) {
    Serial.print("Perform lift position: ");
    Serial.println(microseconds);
    ttservo.attach(pwmPin, minPWM, maxPWM);
    ttservo.writeMicroseconds(microseconds);
    delay(500);  // Allow time for the servo to move
    ttservo.detach();  // Detach the servo to reset its state
    currentServoPosition = microseconds;  // Update the tracked position
  } else {
    Serial.println("Position out of range");
  }
}

void getLiftPosition() {
  String message = "Servo Position (microseconds): " + String(currentServoPosition);
  sendResponse(message.c_str());
}

void liftDebounceLimitSwitch() {
  unsigned long currentMillis = millis();
  if (limitSwitchLiftInterruptTriggered) {
    if ((currentMillis - limitSwitchLastDebounceTime) > debounceDelay) {
      limitSwitchLiftTriggered = true;
      limitSwitchLiftInterruptTriggered = false;
      limitSwitchLastDebounceTime = currentMillis;
      Serial.print("LiftSwitch is down\n");
    }
  }
}
