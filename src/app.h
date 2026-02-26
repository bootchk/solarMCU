
/*
Parameters of the app.

Referenced by many source files.
*/

// If false, work is LED
#define AppWorkIsMotor 1


// Length of period to drive motor in mSec
//#define AppMotorPulsemSec 100
//#define AppMotorPulsemSec 10
//#define AppMotorPulsemSec 50
#define AppMotorPulsemSec 100
//#define AppMotorPulsemSec 150
//#define AppMotorPulsemSec 1000

/*
Duty cycle on PWM to motor driver.
Controls speed after motor starts.
Startup speed and revolutions depends on driver.

If using an LED to view the pulses, anything less than 100
might not be visible.
*/
#define AppMotorDutyCycle 100

/*
This macro used only for low power timer versus RTC.
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
//#define AppMinVccToWork 220

// For Maxon EC9.2 BLDC motor
// Nominal 3V
#define AppMinVccToWork 240

// For NFP1215 BLDC motor
// Nominal starting voltage 3.5
//#define AppMinVccToWork 340
