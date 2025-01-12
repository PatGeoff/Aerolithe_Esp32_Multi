#ifndef STEPPER_H
#define STEPPER_H

#include <AccelStepper.h>

// External declarations
extern AccelStepper stepper;
extern int switchStateFar;
extern int switchStateClose;

extern volatile bool farLimitTriggered;
extern volatile bool nearLimitTriggered;
extern bool iLoop;
extern bool calibrationDone;
extern const int microsteps;
extern volatile bool stepperMotorEmergencyStop;

// Function declarations
void initializeLimitSwitches();
void performStepperMotorMoveTo(int speed, long position);
void stepperReadSwitches();
//void stepperRunToPosition();
// void stepperGoToCloseSwitch();
// void stepperGoToFarSwitch();
//void performStepperMotorNearLimitCalibration();
void performStepperMotorFarLimitCalibration();
void performStepperMotorFullCalibration();
void debounceLimitSwitches();
void stepperReadSwitches();
void setStepperZeroRef();

#endif  // STEPPER_H
