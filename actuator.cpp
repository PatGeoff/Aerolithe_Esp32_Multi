// actuator.cpp

#include "actuator.h"
#include "aerolitheUdp.h"

int hallState_1;
int hallState_2;
// Variables to store sensor states
volatile bool sensor1State = false;
volatile bool sensor2State = false;

bool actuatorSetZeroDone = false;

// Constants for pulse-to-mm conversion
float offset_mm = 0.0;           // Offset for position calibration
const float pulsesPerMM = 17.4;  // Pulses per millimeter
volatile long pulseCount = 0;    // Pulse count from sensors
const int interval = 500;        // Display position every x seconds

unsigned long previousMillis = 0;  // For timing

// Variables to track movement direction
volatile bool isMovingUp = false;
volatile bool isMovingDown = false;

// Initialize the relay pins
void initializeActuator() {
  pinMode(RELAY_PIN_UP, OUTPUT);
  pinMode(RELAY_PIN_DOWN, OUTPUT);

  // Ensure both relays are off initially
  digitalWrite(RELAY_PIN_UP, LOW);
  digitalWrite(RELAY_PIN_DOWN, LOW);

  // Initialize the hall sensor pins
  pinMode(HALL1_PIN, INPUT);  // Set Hall1 pin as input
  pinMode(HALL2_PIN, INPUT);  // Set Hall2 pin as input

  // Attach interrupts to the hall sensor pins
  attachInterrupt(digitalPinToInterrupt(HALL1_PIN), sensor1Interrupt, CHANGE);
  attachInterrupt(digitalPinToInterrupt(HALL2_PIN), sensor2Interrupt, CHANGE);
}

// Function to move the actuator up
void actuatorUp(unsigned long duration) {

  // Set movement direction
  isMovingDown = false;
  isMovingUp = true;
  // Turn on the relay to move the actuator up
  digitalWrite(RELAY_PIN_UP, HIGH);
  delay(duration);                  // Keep the relay on for the specified duration
  digitalWrite(RELAY_PIN_UP, LOW);  // Turn off the relay
  isMovingUp = false;
}

// Function to move the actuator down
void actuatorDown(unsigned long duration) {

  // Set movement direction
  isMovingDown = true;
  isMovingUp = false;

  // Turn on the relay to move the actuator down
  digitalWrite(RELAY_PIN_DOWN, HIGH);
  delay(duration);                    // Keep the relay on for the specified duration
  digitalWrite(RELAY_PIN_DOWN, LOW);  // Turn off the relay

  // Reset movement direction
  isMovingDown = false;
}

void readHalls() {
  hallState_1 = digitalRead(HALL1_PIN);
  hallState_2 = digitalRead(HALL2_PIN);
  // For debugging purposes
  //Serial.print(hallState_1);
  //Serial.print(" ");
  //Serial.println(hallState_2);
}

void IRAM_ATTR sensor1Interrupt() {
  sensor1State = digitalRead(HALL1_PIN);
  updatePosition();
}

// Interrupt service routine for sensor 2
void IRAM_ATTR sensor2Interrupt() {
  sensor2State = digitalRead(HALL2_PIN);
  updatePosition();
}

// Function to update position based on sensor states
void updatePosition() {
  static bool lastSensor1State = LOW;
  static bool lastSensor2State = LOW;

  // Detect rising edge on sensor 1
  if (sensor1State && !lastSensor1State) {
    if (isMovingUp) {
      pulseCount++;
    } else if (isMovingDown) {
      pulseCount--;
    }
  }

  // Detect rising edge on sensor 2
  if (sensor2State && !lastSensor2State) {
    if (isMovingUp) {
      pulseCount++;
    } else if (isMovingDown) {
      pulseCount--;
    }
  }

  lastSensor1State = sensor1State;
  lastSensor2State = sensor2State;
}

// Function to set an offset for calibration
void setOffset(float newOffset_mm) {
  offset_mm = newOffset_mm;
}

// Function to calculate the exact position
float calculatePosition() {
  float currentPosition_mm = pulseCount / pulsesPerMM;
  return currentPosition_mm + offset_mm;  // Calculate exact position
}

// Function to zero the position
void actuatorSetZeroPosition() {
  noInterrupts();  // Disable interrupts while changing pulseCount
  pulseCount = 0;
  offset_mm = 0.0;
  interrupts();  // Re-enable interrupts
  actuatorSetZeroDone = true;
}

void actuatorPosition() {
  // Non-blocking timing
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Save the last time you printed the position
    previousMillis = currentMillis;

    //Print the current position
    Serial.print("Current Position (mm): ");
    Serial.println(calculatePosition()); // en pouces
    Serial.print("Current Position (pouces): ");
    Serial.println(calculatePosition() / 25.4); // en pouces
  }
}

void actuatorGoToPosition(float targetPosition_mm) {
  float currentPosition = calculatePosition();
  if (currentPosition < targetPosition_mm) {
    while (calculatePosition() < targetPosition_mm) {
      actuatorUp(100);  // Adjust duration as necessary
    }
  } else {
    while (calculatePosition() > targetPosition_mm) {
      actuatorDown(100);  // Adjust duration as necessary
    }
  }
}

void actuatorGoTo5degres() {
  // Convert degrees to mm if necessary, here assuming 5 degrees corresponds to a known position in mm
  float targetPosition_mm = 1.0;  // Example target position in mm
  actuatorGoToPosition(targetPosition_mm);
  //actuatorSetZeroPosition();  /// Pourrait être problématique - à valider
}

void actuatorGoTo25degres() {
  // Convert degrees to mm if necessary, here assuming 5 degrees corresponds to a known position in mm
  float targetPosition_mm = 120.0;  // Example target position in mm
  actuatorGoToPosition(targetPosition_mm);
}

void actuatorGoTo45degres() {  // la Current Position devrait être à 225.52
  // Convert degrees to mm if necessary, here assuming 5 degrees corresponds to a known position in mm
  float targetPosition_mm = 212.0;  // Example target position in mm
  actuatorGoToPosition(targetPosition_mm);
}
