// MSP430Drivers lib
#include "msp430Drivers/src/LED/blinkingLED.h"

#include "work.h"

#include "motor.h"

#include "appConfig.h"   // configuration of work

bool 
Work::doWork(void) {
  // Change appConfig.h def of AppWorkIsMotor to choose type of work.

#if AppWorkIsMotor
  // Work is drive small motor a few revs
  return Motor::driveAFewRevs();
#else
  // Work is blink an LED, the red LED on LaunchPad
  BlinkingLED::blinkFirst();
  return true;
#endif
};

void 
Work::doNotWork(void) {
    // Do nothing.  Reserved for future side effects
};
