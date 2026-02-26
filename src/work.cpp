// MSP430Drivers lib
#include "msp430Drivers/src/LED/blinkingLED.h"

#include "work.h"

#include "motor.h"

void 
Work::doWork(void) {
  /*
  Uncomment one to choose the work.
  */


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
