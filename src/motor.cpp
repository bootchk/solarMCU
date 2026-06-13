
#include "motor.h"

#include "msp430Drivers/src/PWM/PWM.h"
#include "msp430Drivers/src/delay/delay.h"
#include "msp430Drivers/src/ADC/adc.h"

#include "energy.h"
#include "dutyCycleMotor.h"
#include "app.h"

/*
Drive a small 4mm diameter motor.
Having a rated voltage of 1.3 - 1.6V

Since Vcc varies from 2.2V to 2.7
when we start driving motor,
we PWM duty cycle Vcc to the motor to reduce the voltage.

We are still overdriving the motor with a high peak voltage.
But the motor wire insulation will tolerate it.
And the average voltage will not stress the motor thermally.

We expect the motor load to drop Vcc.
Since storage capacitor is small.

Vcc drop may be as much as .7V.
We don't drive motor when Vcc drops to 1.9V to avoid MCU brownout.

Duty cycled average voltage could be as low as 1V
when Vcc is 1.9V.
But the motor will be spinning then,
and 1V should still keep motor turning.

We could change the duty cycle as Vcc drops but we don't.
*/

// Require motor is on pin configured for PWM



void     
Motor::turnOn(uint16_t motorDutyCycle)
{
    PWM::turnOn(motorDutyCycle);
}

void 
Motor::turnOff(void)
{
    PWM::turnOff();
    // Assert the timer is not counting.
    // Assert the pin is in low state.
}



void
Motor::driveAFewRevs(void)
{
    /*
    In this design, Vcc varies by light condition.
    Vcc is not regulated.
    But Vcc affects motor speed.
    */
#ifdef AppMotorIsDC1_3
    // For DC motor, scale duty cycle
    Motor::turnOn( DutyCycleMotor::scaledToVcc() );
#else
    // TODO For BLDC, also scale duty cycle to Vcc

    // Unscaled duty cycle
    Motor::turnOn( AppMotorDutyCycle );
#endif

    /*
    We don't sleep in LPM
    because the tens of mA current of motor dominates
    the 100uA current of the mcu.
    */
    
    //Pulse length experimentally determined for the specific motor

    // A simple delay risks exhausting energy and booting MCU.
    // Delay:: inMilliseconds(AppMotorPulsemSec);

    /* Loop, monitoring Vcc after every mSec. */
    for (int i = AppMotorPulsemSec; i > 0; i--)
    {
        if (Energy::isEnoughToKeepWork()){
            Delay:: inMilliseconds(1);
        }
        else {
            // Energy near exhausted.  Stop driving motor.
            Motor::turnOff();
        }
    }

    Motor::turnOff();
}
