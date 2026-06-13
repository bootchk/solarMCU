
// implementation uses ADC on Vcc
#include "msp430Drivers/src/ADC/adc.h"

#include "dutyCycleMotor.h"

#include "app.h"


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
*/


/*
We are PWM the motor at above its typical voltage.
Voltage drop across the motor is very little,
it is the current against the impedance that matters.
PWM gives an average current that is less than
the Max current rating of the motor.

Speed depends on voltage.
We don't care to obtain a high speed.
We only want to start and turn the motor a few revs.

For the DC 1.3V motor, the stall current 
(not to be exceeded for a long time)
is 85 ma.

Experimentally, using oscilloscope.
this duty cycle does not overdrive the motor.
*/

/* We are only choosing the duty cycle once.
As Vcc drops, we could change the duty cycle.
*/

/*
Schedule one scales from max Vcc of 3.6 to about 1.6. 

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
DutyCycleMotor::scaledToVcc()
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

#elif defined(DUTY_CYCLE_SCHEDULE_2)

/*
Schedule 2
Scales voltage to 1.3-1.4

38%        3.4 ++      1.3++
41%        3.2-3.4     1.3 -1.4
43%        3.0-3.2     "
46%        2.8-3.0     "
50%        2.6-2.8     "
54%        2.4-2.6     "
59%        2.2-2.4     "
*/
uint16_t
DutyCycleMotor::scaledToVcc()
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
    else if (centiVolts > 220)
        result = 59;  
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
  #warning "Motor duty cycle not defined"
#endif
