
/*
Parameters of the app.

Referenced by many source files.
*/

// If false, work is LED
#define AppWorkIsMotor 1

// Choose one motor
#define AppMotorIsDC1_3
//#define AppMotorIsMaxonEC9_2
//#define AppMotorIsNFP1215

/* Choosing motor driver parameters

1. Length of period to drive motor in mSec

2. Duty cycle of PWM (i.e. motor speed.)

For DC motor, duty cycle lowers the perceived voltage.
But duty cycle is not defined here, but dynamically scaled to Vcc.

For BLDC motor, the driver chip controls the speed.

Controls speed after motor starts.
Startup speed and revolutions depends on driver.

If using an LED to view the pulses, anything less than 100
might not be visible.
*/

//#define AppMotorPulsemSec 10
//#define AppMotorPulsemSec 50
//#define AppMotorPulsemSec 100
//#define AppMotorPulsemSec 150
//#define AppMotorPulsemSec 1000

#ifdef AppMotorIsDC1_3
// Experimentally, 10 mSec is about one rev
// 20 mSec is a few revs
// 20mSec with storage of 2mF droops Vcc 0.6V
//#define AppMotorPulsemSec 20
#define AppMotorPulsemSec 10
#endif
#ifdef AppMotorIsNFP1215
#define AppMotorPulsemSec 100
#define AppMotorDutyCycle 20
#endif


/*
This macro used only when using low power timer
and not when using RTC.
*/
//#define AppInterWorkPeriod LowPowerTimer::delayThreeSeconds
//#define AppInterWorkPeriod LowPowerTimer::delay48Seconds
#define AppInterWorkPeriod LowPowerTimer::delayFiveSeconds

/*
Vcc at which enough energy to work.

In centivolts.
*/
// For DC motor
// Nominal 1.3V
#define AppMinVccToWork 240

// For Maxon EC9.2 BLDC motor
// Nominal 3V
// #define AppMinVccToWork 240

// For NFP1215 BLDC motor
// Nominal starting voltage 3.5
//#define AppMinVccToWork 340
// NFP215 on Launchpad, whose Vcc is 3.3V nominal, 3.2 actual
//#define AppMinVccToWork 320

