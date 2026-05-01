/* 
Motor parameters
To drive the motor for the use case:
drive the motor a few revs under a low load at low speed.

This describes the use case.
This also describes the motor in a simple way:
the lowest drive parameters.

If the use case is more revs, multiply the duration?
If the use case is higher speed, increase the duty cycle.

1. Length of period to drive motor in mSec
2. Duty cycle of PWM (i.e. motor speed.)
See also below, the min voltage.

For DC motor, duty cycle lowers the perceived voltage.
But duty cycle is not defined here, but dynamically scaled to Vcc.

For BLDC motor, the driver chip controls the speed
from duty cycle, after startup.
Controls speed after motor starts.
Startup speed and revolutions depends on driver chip.

If using an LED to view the pulses, anything less than 100
might not be visible.

The values are experimentally determined to be enough
to start the motor and get a few revs,
from known storage,
without dropping Vcc too much.
*/

//#define AppMotorPulsemSec 10
//#define AppMotorPulsemSec 50
//#define AppMotorPulsemSec 100
//#define AppMotorPulsemSec 150
//#define AppMotorPulsemSec 1000

#ifdef AppMotorIsDC1_3
// Experimentally:
// 10 mSec is one rev
// 20 mSec is a few revs
// 20mSec with storage of 2mF droops Vcc 0.6V
//#define AppMotorPulsemSec 20
#define AppMotorPulsemSec 10
#endif

#ifdef AppMotorIsNFP1215
#define AppMotorPulsemSec 100
#define AppMotorDutyCycle 20
#endif

#ifdef AppMotorIsNidec6s
// Experiment: duration 10, speed 20 turns a fraction of one rev @5.7V
// Duration
#define AppMotorPulsemSec 100
// Speed
#define AppMotorDutyCycle 20
#endif





/*
Vcc at which enough energy to work.
For implementations that monitor Vcc with ADC, this is a voltage threshold.

In centivolts.

Where work is a DC or BLDC motor.
This doesn't properly set AppMinVccToWork
when work is flashing an LED and choice of motor is not defined.
*/
#ifdef AppMotorIsDC1_3
// 4mm diameter DC motor, pager motor.
// Nominal 1.3V
#define AppMinVccToWork 240
#endif
#ifdef AppMotorIsMaxonEC9_2
// For Maxon EC9.2 BLDC motor
// Nominal 3V
#define AppMinVccToWork 240
#endif
#ifdef AppMotorIsNFP1215
// For NFP1215 BLDC motor
// Nominal starting voltage 3.5
#define AppMinVccToWork 340
// NFP215 on Launchpad, whose Vcc is 3.3V nominal, 3.2 actual
#define AppMinVccToWork 320
#endif
#ifdef AppMotorIsNidec6s
// For Nidec 6s BLDC motor rated 12V, but starts at 5V
// !!! Not used, instead an external voltage monitor is used, 
// see energy.cpp
#define AppMinVccToWork 500
#endif
