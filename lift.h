// Lift.h

#ifndef LIFT_H
#define LIFT_H

extern volatile bool limitSwitchLiftInterruptTriggered;
extern volatile int position;
extern volatile bool canGoDown;
extern volatile bool canGoUp;

void liftServoSetup();
void liftServoSetZero();
void liftPerformSetPosition(const int pos);
void onLiftLimitSwitchTriggered();
void liftCalibrateLimitSwitch();
void liftDebounceLimitSwitch();
void getLiftPosition();

#endif // LIFT_H