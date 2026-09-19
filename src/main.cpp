/*
This main waits in LPM3.5

Derived from TI code.

This main has fewer abstractions.
Uses "device register" level of code, not DriverLib.

The abstraction used is: app work.
*/
#include <cstdint>
#include <msp430.h>


// msp430drivers
#include "msp430Drivers/src/periodicInterrupt/periodicInterrupt.h"
#include "msp430Drivers/src/motorControl/motorSpeedFeedback.h"
#include "msp430Drivers/src/SoC/SoC.h"


#include "appConfig.h"
#include "app.h"

#include "energy.h"


// This configuration will not take effect until unlock LPM5
// The configuration MUST be as left it when entered LPM5
void initGpio(void)
{
    // Initially GPIO pins are outputs, low
    P1DIR = 0xFF; P2DIR = 0xFF;
    // with no pullup.
    P1REN = 0xFF; P2REN = 0xFF;
    // value low
    P1OUT = 0x00; P2OUT = 0x00;

#ifdef __MSP430FR2433__
    P3DIR = 0xFF;
    P3REN = 0xFF;
    P3OUT = 0x00;
#endif

    // Then reconfigure used pins to their desired config

    // Assert PWM pin to motor is low, driver IC is in standby
    
    // For some implementations, init input pin used to monitor energy availability.
    Energy::initPin();

    MotorSpeedFeedback::initPinsForSingleTurn();
    // MotorSpeedFeedback::initPinsForTurnCount();
}


/* Init RTC device.

Only done on coldstart, see call below.
The RTC device stays powered in LPM3.5
and does not require configuration after wake reset.

We don't start and stop the RTC.
Interrupts are periodic and continue.
*/

void initRTC(void)
{
    PeriodicInterrupt::initInSeconds(AppSleepPeriodInSeconds);
}






int main(void)
{
    // Regardless of how we wake up, 
    // stop the WDT and configure GPIO before doing anything else.
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    initGpio();                             // Configure GPIO

     // Using FRAM for persistent state variables through LPM3.5
    SoC::disableFRAMWriteProtect();

    // First determine whether we are coming out of an LPMx.5 or a regular RESET.
    if (SYSRSTIV == SYSRSTIV_LPM5WU)        // When woken up from LPM3.5, reinit
    {
        // MCU wakes up from LPM3.5
        
        // re-init GPIO.  RTC is still powered and does not need configuration.
        
        // Disable the GPIO power-on default high-impedance mode
        // to activate previously configured port settings
        PM5CTL0 &= ~LOCKLPM5;

        __enable_interrupt();               
        // The RTC interrupt should trigger now.
        // The ISR does NOT toggle the LED

        App::work();

        // enter LPM3.5 again.
    }
    else
    {
        // Device powered up from a cold start.

        // activate GPIO configuration
        PM5CTL0 &= ~LOCKLPM5;

        // Configure RTC to interrupt every interwork period.
        // Only done once, on coldstart.
        // The RTC will request and use the VLO, which stays on.
        initRTC();
        // GIE is off: no interrupts yet

        App::coldStart();
    }

    // Enter LPM3.5 mode with interrupts enabled. Note that this operation does
    // not return. The LPM3.5 will exit through a RESET event, resulting in a
    // re-start of the code.
    PMMCTL0_H = PMMPW_H;                    // Open PMM Registers for write
    PMMCTL0_L |= PMMREGOFF;                 // and set PMMREGOFF
    __bis_SR_register(LPM3_bits | GIE);
    __no_operation();

    return 0;
}



// See ISR.cpp for RTC interrupt handler
// It does nothing substantive.
