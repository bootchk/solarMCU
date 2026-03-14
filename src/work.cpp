// MSP430Drivers lib
#include "msp430Drivers/src/LED/blinkingLED.h"

#include "work.h"

#include "motor.h"

#include "app.h"   // configuration of work

void 
Work::doWork(void) {
  // Change app.h def of AppWorkIsMotor to choose type of work.

#if AppWorkIsMotor
// Work is drive small motor a few revs
  // Accounts for varying Vcc
  Motor::driveAFewRevs();
#else
  // Work is blink an LED, the red LED on LaunchPad
  BlinkingLED::blinkFirst();
#endif
};

void 
Work::doNotWork(void) {
    // Do nothing.  Reserved for future side effects
};
