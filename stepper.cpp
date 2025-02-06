// stepper.cpp

// When we start the app, the camera will be in either of these states:  the furthest from the turntable and triggering the far limit switch,
// the closest and triggering the near limit switch or somewhere in between triggering no switch at all.
// I order to move the camera by commands with Aerolithe, a full calibration needs to be done. Only after will the trackbar be enabled and scanning routines too.
// The calibration will first go to the furthest from the turntable and trigger the far limit switch. This will set the stepper current position to zero with stepper.setCurrentPosition(0).
// With the 250mm alunminium extrusion rail setiup for the camera, when far limit switch is triggered and set to zero, having 60 000 steps brings the camera all the way to about a centimeter before the near limit switch.
// The Aerolithe max value will be 60 000 steps but if ever the camera is in the middle of the rail, for example, and the power goes off, this will be the new zero so if we were to give it a command of 60 000 afterwards, it will
// reach the far limit switch and reset to zero.


#include "stepper.h"
#include <WiFiUDP.h>
#include "aerolitheUdp.h"

const int stepPin = 12;       // Pin connected to the STEP pin of the stepper driver
const int dirPin = 13;        // Pin connected to the DIR pin of the stepper driver
const int farLimitPin = 4;    // Pin for the limit switch when the camera moves away
const int nearLimitPin = 15;  // Pin for the limit switch when the camera moves closer

const int microsteps = 16;  // microsteps à 1/16
const int acceleration = 10000;

const long maxPosition = 61200;  // Maximum allowable position
const int maxSpeed = 6000;       // Maximum speed variable
volatile int previousSpeed = 0;
bool runSpeedBool = false;


// An instance of AccelStepper
AccelStepper stepper(AccelStepper::DRIVER, stepPin, dirPin);

// Define variables for the limit switch flags
volatile bool farLimitInterruptTriggered = false;
volatile bool nearLimitInterruptTriggered = false;

volatile bool farLimitTriggered = false;
volatile bool nearLimitTriggered = false;

const int debounceDelay = 50;         // Debounce delay in milliseconds
const int debounceDelayStuck = 1000;  // Debounce delay in milliseconds
unsigned long farLimitLastDebounceTime = 0;
unsigned long farLimitLastDebounceTimeStuck = 0;
unsigned long nearLimitLastDebounceTime = 0;

bool allowMoveForward = true;
bool allowMoveBackward = true;


bool calibrationDone = false;
volatile bool stepperMotorEmergencyStop = false;

long stepperMaxPosition = 61287;  // Depending on the lenght of the rail and the position of the switch, this value will have to be adjusted. Uncomment Serial.print("Extreme Far position: " + (String)stepper.currentPosition()); in performStepperMotorFarLimitCalibration(). This value includes the microsteps multiplicator
long stepperCurrentPosition = 0;  // On calibration, the far limit switch needs to be triggered. Then it is set to zero.


// Interrupt service routines
// NOT A GOOD IDEA to put processor heavy fuctions in the IRAM_ATTR(), keep it at minimum
void IRAM_ATTR onFarLimit() {
  farLimitInterruptTriggered = true;
  allowMoveBackward = false;
}

void IRAM_ATTR onNearLimit() {
  nearLimitInterruptTriggered = true;
  allowMoveForward = false;
}

void initializeStepperLimitSwitches() {
  // Set up the pins
  pinMode(farLimitPin, INPUT_PULLUP);
  pinMode(nearLimitPin, INPUT_PULLUP);

  // Attach interrupts to the limit switch pins
  attachInterrupt(digitalPinToInterrupt(farLimitPin), onFarLimit, FALLING);
  attachInterrupt(digitalPinToInterrupt(nearLimitPin), onNearLimit, FALLING);

  stepperReadSwitches();
}

void debounceLimitSwitches() {
  unsigned long currentMillis = millis();
  static bool farLimitMessagePrinted = false;
  static bool nearLimitMessagePrinted = false;

  if (farLimitInterruptTriggered) {
    if ((currentMillis - farLimitLastDebounceTime) > debounceDelay) {
      farLimitTriggered = true;
      farLimitInterruptTriggered = false;
      farLimitLastDebounceTime = currentMillis;

      if (!farLimitMessagePrinted) {
        Serial.println("Debounce -> Far Limit Switch Pressed");
        farLimitMessagePrinted = true;
        stepper.setCurrentPosition(0);
      }
    }
  } else {
    farLimitMessagePrinted = false;
  }

 if (nearLimitInterruptTriggered) {
    if ((currentMillis - nearLimitLastDebounceTime) > debounceDelay) {
      nearLimitTriggered = true;
      nearLimitInterruptTriggered = false;
      nearLimitLastDebounceTime = currentMillis;

      if (!nearLimitMessagePrinted) {
        Serial.println("Debounce -> Near Limit Switch Pressed");
        nearLimitMessagePrinted = true;
      }
    }
  } else {
    nearLimitMessagePrinted = false;
  }
}


void performStepperMotorFarLimitCalibration() {
  farLimitTriggered = digitalRead(farLimitPin) == LOW;
  nearLimitTriggered = digitalRead(nearLimitPin) == LOW;

  if (farLimitTriggered) {
    Serial.print("Stepper -> Far Limit Switch Pressed");
    stepper.setCurrentPosition(0);
    stepperCurrentPosition = 0;
    stepper.setMaxSpeed(maxSpeed);
    stepper.setAcceleration(acceleration);
    stepper.moveTo(1000);  // Move 2000 away from the limit switch in the positive direction
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
    stepperCurrentPosition = stepper.currentPosition();
    delay(200);
    stepper.setCurrentPosition(0);
    farLimitTriggered = false;  // Reset the flag after moving away
    stepper.moveTo(30000);
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
    stepperCurrentPosition = stepper.currentPosition();
  } else if (nearLimitTriggered) {
    Serial.print("Stepper -> Near Limit Switch Pressed");
    stepper.setCurrentPosition(95000);  //
    stepper.setMaxSpeed(maxSpeed);
    stepper.moveTo(-20);  // Move 2000 away from the limit switch in the positive direction
    while (stepper.distanceToGo() != 0) {
      stepper.run();
    }
    performStepperMotorFarLimitCalibration();
  } else {
    stepper.setCurrentPosition(95000);
    performStepperMotorMoveTo(maxSpeed, 0);
    stepper.moveTo(1000);
    stepper.setCurrentPosition(0);
    stepper.moveTo(30000);
  }
}

// Position is 0 to 61200 ??

// void performStepperMotorMoveTo(int speed, long position) {

//   debounceLimitSwitches();  // Ensure we debounce the switches before starting calibration
//   farLimitTriggered = digitalRead(farLimitPin) == LOW;
//   nearLimitTriggered = digitalRead(nearLimitPin) == LOW;


//   if ((position > stepper.currentPosition() && allowMoveForward) || (position < stepper.currentPosition() && allowMoveBackward)) {
//     if (position <= maxPosition) {
//       stepper.setMaxSpeed(speed);
//       stepper.setAcceleration(acceleration);
//       stepper.moveTo(position);
//     }
//   }

//   // If the far limit switch is triggered, move slightly away from it in the positive direction
//   if (farLimitTriggered) {
//     stepper.setMaxSpeed(maxSpeed);
//     stepper.setAcceleration(acceleration);
//     stepper.moveTo(200 * microsteps);  // Move 200 steps away from the limit switch in the positive direction
//     while (stepper.distanceToGo() != 0) {
//       stepper.run();
//     }
//     delay(200);
//     farLimitTriggered = false;  // Reset the flag after moving away
//     Serial.println("Far Limit Reached. Moving away");
//   }

//   // If the near limit switch is triggered, move slightly away from it in the negative direction
//   if (nearLimitTriggered) {
//     stepper.setMaxSpeed(maxSpeed);
//     stepper.setAcceleration(acceleration);
//     stepper.moveTo(-20 * microsteps);  // Move 200 steps away from the limit switch in the negative direction
//     while (stepper.distanceToGo() != 0) {
//       stepper.run();
//     }
//     delay(200);
//     nearLimitTriggered = false;  // Reset the flag after moving away
//   } else {
//     // Proceed with the main movement
//     stepper.setMaxSpeed(speed);
//     stepper.setAcceleration(acceleration);
//     stepper.moveTo(position);
//     while (stepper.distanceToGo() != 0) {
//       stepper.run();
//     }
//     delay(200);
//   }
// }

void performStepperMotorMoveTo(int speed, long position) {
  debounceLimitSwitches();  // Ensure we debounce the switches before starting the movement

  if ((position > stepper.currentPosition() && allowMoveForward) || (position < stepper.currentPosition() && allowMoveBackward)) {
    if (position <= maxPosition) {
      stepper.setMaxSpeed(speed);
      stepper.setAcceleration(acceleration);
      stepper.moveTo(position);
    } else {
      Serial.println("Stepper -> Requested position exceeds maximum allowable position.");
    }
  } else {
    Serial.println("Stepper -> Movement not allowed in the requested direction.");
  }

  // Call run() method to make the stepper move asynchronously
  long delta = abs(position - stepper.currentPosition());
  static long stepsToRun = 0;

  if (stepsToRun == 0) {
    stepsToRun = delta;
  }

  if (stepsToRun > 0 && !stepperMotorEmergencyStop) {
    stepper.run();
    stepperCurrentPosition = stepper.currentPosition();  // Update the current position
    stepsToRun--;
  }
}

void performStepperMotorRunSpeed(int speed) {
  runSpeedBool = true;
  Serial.print("Stepper -> Run Speed Start");
  stepper.setMaxSpeed(maxSpeed);
  stepper.setAcceleration(acceleration);
  stepper.setSpeed(speed);
}


void stepperReadSwitches() {
  farLimitTriggered = digitalRead(farLimitPin) == LOW;
  nearLimitTriggered = digitalRead(nearLimitPin) == LOW;

  Serial.println("Stepper -> farLimit switch pressed? " + (String)farLimitTriggered);
  Serial.println("Stepper -> nearLimit switch pressed? " + (String)nearLimitTriggered);
}

void setStepperZeroRef() {
  stepper.setCurrentPosition(0);
}

void setStepperMaxPosRef() {
  stepper.setCurrentPosition(61200);
}
