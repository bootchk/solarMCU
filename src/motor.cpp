
#include "motor.h"

#include "msp430Drivers/src/PWM/PWM.h"
#include "msp430Drivers/src/delay/delay.h"
#include "msp430Drivers/src/ADC/adc.h"

#include "app.h"

/*
Drive a small 4mm diameter motor.
Having a rated voltage of 1.3V

Since Vcc varies from 2.2V to 2.7
when we start driving motor,
we PWM duty cycle Vcc to the motor to reduce the voltage.

We are still overdriving the motor.

We could change the duty cycle as Vcc drops but we don't.

We expect the motor load to drop Vcc about 0.3V.
Since storage capacitor is small.
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

/*
Returns a dutyCycle that gives 
the desired voltage on the motor, about 1.3V.
Based on Vcc now, which varies.
We turn the motor when Vcc > 2.2V.
Vcc must remain above 1.8V to avoid brownout.


Vcc should drop 0.3V by motor load
50% of Vcc 2.7->2.4 is motor 1.35->1.2
52%        2.6->2.3
54%        2.5->2.2      
56%        2.4->2.1
59%        2,2->1.9
*/
uint16_t
motorDutyCycleScaledToVcc()
{
    uint16_t result = 0;

    unsigned int centiVolts = ADC::measureVccCentiVolts();

    if (centiVolts > 270)
        result = 50;
    else if (centiVolts > 240)
        result = 56;
    else if (centiVolts > 220)
        result = 59;
    else
        // Not turn motor when Vcc is too low.
        result = 0;
    return result;
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
    Motor::turnOn( motorDutyCycleScaledToVcc() );
#else
    // TODO For BLDC, also scale duty cycle to Vcc

    // Unscaled duty cycle
    Motor::turnOn( AppMotorDutyCycle );
#endif

    /*
    We don't sleep in LPM
    because the tens of mA current of the motor dominates
    the 100uA current of the mcu
    */
    
    // Pulse length experimentally determined for the specific motor
    Delay:: inMilliseconds(AppMotorPulsemSec);

    Motor::turnOff();
}
