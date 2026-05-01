
/*
Parameters of the app.
This describes variants of the app.
The app is always: every period, work i.e. drive motor a few revs
if energy permits.

Referenced by many source files.

board.h must define a board that supports the app choices.
E.g. not all boards have a separate high rail.
E.g. some boards the PWM is too a motor driver IC.
*/

// If false, work is LED
// A hack to test with an LED instead of a motor.
#define AppWorkIsMotor 1

// Choose one motor
#define AppMotorIsDC1_3
//#define AppMotorIsMaxonEC9_2
//#define AppMotorIsNFP1215
//#define AppMotorIsNidec6s

// Configure what rail to monitor for energy availability, and how to monitor it.
// Set one of these to 1
#define EnergyFromVcc       1
#define EnergyFromVHighRail 0


#define AppInterWorkPeriodInSeconds 48


// Must follow choice of motor.
#include "motorParams.h"


/*
This macro was used when using low power timer (say WDT)
and not when using RTC.
CRUFT
*/
//#define AppInterWorkPeriod LowPowerTimer::delayThreeSeconds
//#define AppInterWorkPeriod LowPowerTimer::delay48Seconds
// #define AppInterWorkPeriod LowPowerTimer::delayFiveSeconds
