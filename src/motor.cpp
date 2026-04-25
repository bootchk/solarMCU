
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
the desired voltage on the motor.

Desired voltage on motor is less than 1.6V.
The motor is rated nominal 1.3V, operating 1.1-1.6V
That is the voltage for a certain high speed under load.

We are starting the motor.
We apply a slightly higher voltage.
That increases likelihood of starting?
The motor will tolerate it, for a short time.
We are pulsing the motor.
If we were running the motor for a long time,
we would reduce the duty cycle (perceived voltage)
a short time after starting the motor.

Based on Vcc now, which varies.
We turn the motor when Vcc > 2.2V.
Vcc must remain above 1.8V to avoid brownout of MCU

Vcc on storage capacitor should droop no more than 0.3V
when we work (by discharge current through motor).
The pulse length and duty cycle affect the energy used.

Under all conditions (of light on solar cell power source)
Vcc should not exceed 3.6V

Typically, at around 100lux, Voc of solar cell reaches 2.7V

Duty 
Cycle    Vcc range   Integrated voltage to motor
44%        3.4 ++      1.5
47%        3.2-3.4     1.5 -1.6
50%        3.0-3.2     1.5 -1.6
53%        2.8-3.0     1.5 -1.6
57%        2.6-2.8     1.5 -1.6
62%        2.4-2.6     1.5 -1.6    
66%        2.2-2.4     1.45-1.6
           0  -2.0     0, not driving
*/
#ifdef DUTY_CYCLE_SCHEDULE_1
uint16_t
motorDutyCycleScaledToVcc()
{
    uint16_t result = 0;

    unsigned int centiVolts = ADC::measureVccCentiVolts();

    if (centiVolts > 340)
        result = 44;
    else if (centiVolts > 320)
        result = 47;
    else if (centiVolts > 300)
        result = 50;
    else if (centiVolts > 280)
        result = 53;
    else if (centiVolts > 260)
        result = 57;
    else if (centiVolts > 240)
        result = 62;
    //else if (centiVolts > 220)
    //   result = 66;  
    else
        // Not turn motor when Vcc is too low.
        result = 0;

    // Repect the constant defined in app.h
    // No matter what the code above does.
    if (centiVolts < AppMinVccToWork)
        result = 0;
    
    return result;
}

#else

/*
Schedule 2
38%        3.4 ++      1.3++
41%        3.2-3.4     1.3 -1.4
43%        3.0-3.2     "
46%        2.8-3.0     "
50%        2.6-2.8     "
54%        2.4-2.6     "
*/
uint16_t
motorDutyCycleScaledToVcc()
{
    uint16_t result = 0;

    unsigned int centiVolts = ADC::measureVccCentiVolts();

    if (centiVolts > 340)
        result = 38;
    else if (centiVolts > 320)
        result = 41;
    else if (centiVolts > 300)
        result = 43;
    else if (centiVolts > 280)
        result = 46;
    else if (centiVolts > 260)
        result = 50;
    else if (centiVolts > 240)
        result = 54;
    //else if (centiVolts > 220)
    //   result = 66;  
    else
        // Not turn motor when Vcc is too low.
        result = 0;

    // Repect the constant defined in app.h
    // No matter what the code above does.
    if (centiVolts < AppMinVccToWork)
        result = 0;
    
    return result;
}
#endif

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
