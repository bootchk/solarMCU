
#include "motor.h"

#include "msp430Drivers/src/delay/delay.h"
#include "msp430Drivers/src/motorControl/motorControl.h"
#include "msp430Drivers/src/motorControl/motorSpeedFeedback.h"


#include "energy.h"
#include "dutyCycleMotor.h"
#include "appConfig.h"

/*
Drive a small motor.
Use case is to start and turn a few turns.

We expect the motor load to drop Vcc, since storage capacitor is small.
Monitors Vcc while driving motor to ensure not brownout the mcu.
Vcc drop may be as much as .7V.
We don't drive motor when Vcc drops to 1.9V to avoid MCU brownout.

For some motors and driver ICs, we count actual turns,
and can drive a desired number of turns.
For other motors, we drive the motor a certain time
which by experiment gives the desired turns.
For both kinds, we drive the motor no longer than the certain time.

See motorParams.h for typical motors.
This accomodates both DC motors and BLDC motors via a driver chip.
*/

/*
For BLDC motors, duty cycle is the speed via a PWM input to driver IC.
*/

/*
For a small DC motor rated 1.5V, since Vcc varies from 2.2V to 2.7
when we start driving motor,
we PWM duty cycle Vcc to the motor to reduce the voltage.
PWM still overdrives the motor with a high peak voltage.
But the motor wire insulation will tolerate it.
And the average voltage will not stress the motor thermally.

Duty cycled average voltage could be as low as 1V when Vcc is 1.9V.
But the motor will be spinning then,
and 1V should still keep motor turning.

We could change the duty cycle as Vcc drops but we don't.
*/

// Require motor is on pin configured for PWM

// The reason we stopped driving motor.
// Not all reasons apply to all motor kinds.
enum class ReasonMotorOff : unsigned char {
    TimeElapsed,
    TurnedDesired,
    EnergyExhausted
};

void     
Motor::turnOn(uint16_t motorDutyCycle)
{
    MotorControl::start(motorDutyCycle);
}

void 
Motor::turnOff(void)
{
    MotorControl::stop();
    // Assert the PWM to motor is disabled.
    // Assert the pin is in low state.
}


bool
Motor::didTurnDesiredTurns( unsigned int desiredTurns)
{
    return MotorSpeedFeedback::getTurnCount() >= desiredTurns;
}


/*
Vcc varies by light condition.
Vcc is not regulated in all use cases.
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
    unsigned int mSecCount;

    // Default reason
    ReasonMotorOff reasonStoppedMotor = ReasonMotorOff::TimeElapsed;
    
    // For count turns
    MotorSpeedFeedback::enableTurnInterrupt();

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
    for (mSecCount = AppMotorPulsemSec; mSecCount > 0; mSecCount--)
    {
        if (Energy::isEnoughToKeepWork()){
            // Still enough energy
            if (MotorSpeedFeedback::didTurn()) {
                // Motor started at least.

                if (Motor::didTurnDesiredTurns(AppDesiredMotorTurns)){
                    // Quit loop.
                    reasonStoppedMotor = ReasonMotorOff::TurnedDesired ;
                    break;
                }
                else {
                    /*
                    Motor turned at least once, but keep motor on until desired turns.
                    Remain monitor Vcc and timeout.
                    */
                    Delay:: oneMillisecond();
                }

                // Use an oscilloscope on PWM and FG to know timing.
                // Time for motor to start plus time to turn desired count.
                // The motor can turn many turns before FG starts pulsing.
            }
            else {
                 Delay:: oneMillisecond();
            }
        }
        else {
            // Quit loop.
            reasonStoppedMotor = ReasonMotorOff::EnergyExhausted;
            break;
        }
    }
    /*
    reason is set.
    Motor still On.
    */

    MotorSpeedFeedback::disableTurnInterrupt();

    Motor::turnOff();

    /*
    Either reasonStoppedMotor is:
        3 turned desired revs
        2 exhausted energy but might have turned some
        1 attempted turn entire period. Might have turned some.
          This is the case for motors with no feedback.
    */
    /*
    Result is true if feedback from motor said it turned.
    Result false does not imply the motor did not physcially turn.
    */
    return (reasonStoppedMotor == ReasonMotorOff::TurnedDesired);
}
