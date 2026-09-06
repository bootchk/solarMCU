
#include "motor.h"

#include "msp430Drivers/src/PWM/PWM.h"
#include "msp430Drivers/src/delay/delay.h"
#include "msp430Drivers/src/motorControl/motorSpeedFeedback.h"
#include "msp430Drivers/src/i2c/i2c.h"

//#include "msp430Drivers/src/ADC/adc.h"
//#include "msp430Drivers/src/SoC/SoC.h"


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
    unsigned char buffer[3];

    I2C::configureMaster( 1, true);
    I2C::write(1, buffer, 3);
    
    ////PWM::turnOn(motorDutyCycle);
}

void 
Motor::turnOff(void)
{
    ////PWM::turnOff();
    // Assert the timer is not counting.
    // Assert the pin is in low state.
}


/*
Vcc varies by light condition.
Vcc is not regulated.
Vcc affects motor speed.

A period of time determines how many turns.
We don't delay for the entire time, but iterate by mSec.
We stop prematurely when:
   Vcc droops too low
   Motor feedback tells the count of turns
Since we are polling every mSec, the motor may turn more than desired.

Return whether we think the motor turned.
*/
bool
Motor::driveAFewRevs(void)
{

    // Default reason is time elapsed
    int reasonStoppedMotor = 1;
    
    // For counting turns
    // MotorSpeedFeedback::startTurnCounter(1, MOTOR_POLE_PAIRS);
    MotorSpeedFeedback::enableSingleTurnInterrupt();

    // requires GIE enabled.  It should be.
    

#ifdef AppMotorIsDC1_3
    // For DC motor, scale duty cycle
    Motor::turnOn( DutyCycleMotor::scaledToVcc() );
#else
    // For BLDC, the driver IC controls voltage,
    // and duty cycle is just the desired speed.

    // Unscaled duty cycle
    Motor::turnOn( AppMotorDutyCycle );
#endif

    /*
    We don't sleep in LPM
    because the tens of mA current of motor dominates
    the 100uA current of the mcu.
    */
    
    //Pulse length experimentally determined for the specific motor

    /* Loop, polling Vcc and desired turns every mSec. */
    for (int i = AppMotorPulsemSec; i > 0; i--)
    {
        if (Energy::isEnoughToKeepWork()){
            // Still enough energy
            if (MotorSpeedFeedback::wasCountReachedFlag()) {
                // Turned desired turns.
                // Quit loop and stop driving motor.
                Motor::turnOff();
                reasonStoppedMotor = 3;

                // Use an oscilloscope on PWM and FG to know timing.
                // Time for motor to start plus time to turn desired count.
                // The motor turns many turns before FG starts pulsing.
                break;
            }
            else {
                 Delay:: oneMillisecond();
            }
        }
        else {
            // Energy near exhausted.  
            // Quit loop and stop driving motor.
            reasonStoppedMotor = 2;
            break;
        }
    }


    // MotorSpeedFeedback::stopTurnCounter();
    MotorSpeedFeedback::disableSingleTurnInterrupt();
    Motor::turnOff();

    /*
    Either reasonStoppedMotor is:
        3 turned desired, 
        2 exhausted energy but might have turned some
        1 time expired and but might have turned some
    Result is true if feedback from motor said it turned.
    Result false does not imply the motor did not turn.
    */
    return (reasonStoppedMotor == 3);
}
