
/*
Parameters of the app.
This describes variants of the app.
The app is always: every period, work (e.g. drive motor a few revs)
if energy permits.

Referenced by many source files.

board.h must define a board that supports the app choices.
E.g. not all boards have a separate high rail.
E.g. some boards the PWM is too a motor driver IC.

The parameters are:
    AppWorkIsMotor (otherwise LED)
    AppMotorIs: which motor
    EnergyFrom: what rail to monitor for energy availability, and how to monitor it.
    AppSleepPeriodInSeconds
    AppInterWorkIsShort: true if two wait periods between work

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
// App is turn the motor often, the system quickly stabilizes
//#define AppIsStriker 1

// App is turn the motor less often, allow more time for system stabilize
// The physical system is small, and resumes stable position quickly.
//#define AppIsMobileSmall 1

// App is turn motor less often.
// The physical system is large and takes a long time to resume stable position.
//#define AppIsMobileLarge

//#define AppIsBenchTestDC1_3  1
//#define AppIsTestBLDCMaxon 1
//#define AppIsTestBLDCNFP1215 1
#define AppIsVeerBLDCNFP1215 1







#if defined(AppIsStriker)

#define AppWorkIsMotor 1
// small DC motor
#define AppMotorIsDC1_3
#define EnergyFromVcc  1
#define AppSleepPeriodInSeconds 60
// Wait one minute between motor turn
#define AppInterWorkIsOne 1


#elif defined(AppIsMobileSmall)

#define AppWorkIsMotor 1
// small DC motor
#define AppMotorIsDC1_3
#define EnergyFromVcc  1
#define AppSleepPeriodInSeconds 60
// Wait two minutes between motor turn
#define AppInterWorkIsTwo


#elif defined(AppIsMobileLarge)

#define AppWorkIsMotor 1
#define AppMotorIsMaxonEC9_2
#define EnergyFromVcc  1
#define AppSleepPeriodInSeconds 60
#define AppInterWorkIsSix 1


#elif defined(AppIsBenchTestDC1_3)
// Test value using small DC motor
// Turn more often to reduce wait for result.
#define AppWorkIsMotor 1
#define AppMotorIsDC1_3
#define EnergyFromVcc  1
#define AppSleepPeriodInSeconds 5
#define AppInterWorkIsOne 1


#elif defined(AppIsTestBLDCMaxon)

#define AppWorkIsMotor 1
#define AppMotorIsMaxonEC9_2
#define EnergyFromVcc  1
#define AppSleepPeriodInSeconds 15
#define AppInterWorkIsOne 1


#elif defined(AppIsTestBLDCNFP1215)

#define AppWorkIsMotor 1
#define AppMotorIsNFP1215
// Vmotor is same as Vmcu
#define EnergyFromVcc 1
#define AppSleepPeriodInSeconds 15
#define AppInterWorkIsOne 1
#define TestOnLaunchpad   1

#elif defined(AppIsVeerBLDCNFP1215)

#define AppWorkIsMotor 1
#define AppMotorIsNFP1215
// Vmotor is same as Vmcu
#define EnergyFromVcc 1
#define AppSleepPeriodInSeconds 60
// Three minutes between motor turns.
#define AppInterWorkIsThree 1

// Other BLDC motors for other use cases
//#define AppMotorIsNidec6s

#else
#error "appConfig.h does not define AppIs..."
#endif




// Must follow choice of motor.
#include "motorParams.h"
