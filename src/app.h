
/*
Parameters of the app.
This describes variants of the app.
The app is always: every period, work i.e. drive motor a few revs
if energy permits.

Referenced by many source files.

board.h must define a board that supports the app choices.
E.g. not all boards have a separate high rail.
E.g. some boards the PWM is too a motor driver IC.

The parameters are:
    AppWorkIsMotor (otherwise LED)
    AppMotorIs: which motor
    EnergyFrom: what rail to monitor for energy availability, and how to monitor it.
    AppInterWorkPeriodInSeconds

Other parameters are in motorParams.h
*/

/*
!!! Correctness of app also might depend
on correct configuration of msp430Drivers library via its board.h.
E.G. it declares what GPIO pins used for PWM to motor,
or GPIO pin used to enable motor driver IC.
!!! Configuration declarations do not yet flow from this file to board.h
*/

/*
A set of parameters declares the total config of the app.
Choice of motor defines another set of parameters.
*/

#define AppIsProductionDC1_3 1

#if defined(AppIsProductionDC1_3)
// Production value using small DC motor
#define AppWorkIsMotor 1
#define AppMotorIsDC1_3
#define EnergyFromVcc  1
#define AppInterWorkPeriodInSeconds 60

#elif defined(AppIsBenchTestDC1_3)
// Test value using small DC motor
// Turn more often to reduce wait for result.
#define AppWorkIsMotor 1
#define AppMotorIsDC1_3
#define EnergyFromVcc  1
#define AppInterWorkPeriodInSeconds 5

#elif defined(AppIsTestBLDC)

// Other BLDC motors for other use cases
//#define AppMotorIsMaxonEC9_2
//#define AppMotorIsNFP1215
//#define AppMotorIsNidec6s

#define AppWorkIsMotor 1
#define AppMotorIsNFP1215
#define EnergyFromVHighRail 1
#define AppInterWorkPeriodInSeconds 60

#else

#endif




// Must follow choice of motor.
#include "motorParams.h"
