/*
This main waits in LPM3.5

Derived from TI code.

This main has fewer abstractions.
Uses "device register" level of code, not DriverLib.

The abstraction used is: app work.
*/
#include <cstdint>
#include <msp430.h>

#include "workRateFSM.h"
#include "energy.h"



void initGpio(void)
{
    // All GPIO pins are outputs, low, with no pullup.
    P1DIR = 0xFF; P2DIR = 0xFF; 
    P1REN = 0xFF; P2REN = 0xFF; 
    P1OUT = 0x00; P2OUT = 0x00;

#ifdef __MSP430FR2433__
    P3DIR = 0xFF;
    P3REN = 0xFF;
    P3OUT = 0x00;
#endif

    // For some implementations, init input pin used to monitor energy availability.
    Energy::initPin();
}


/* Init RTC device.

Only done on coldstart, see call below.
The RTC device stays powered in LPM3.5
and does not require configuration after wake reset.

We don't start and stop the RTC.
Interrupts are periodic and continue.
*/

// Init RTC to alarm after ticks (of 1024/12000 = .0853 seconds)
void initRTCInTicks(uint16_t ticks)
{
    // Configure RTC
    // VLO clock source @ 12kHz

    // Interrupt and reset happen every 1024/12000 * ticks sec.
    RTCMOD = ticks-1;
    RTCCTL = RTCSS__VLOCLK | RTCSR |RTCPS__1024;

    // Enable interrupt from device, but not the GIE
    RTCCTL |= RTCIE;
}


void initRTC48Seconds(void)
{
    // 512 * .0853 = 43.7 seconds
    initRTCInTicks(512);
}

void initRTC3Seconds(void)
{
    // 32 * .0853 = 2.7seconds
    initRTCInTicks(32);
}

void initRTC(void)
{
    //initRTC3Seconds();
    initRTC48Seconds();
}


// Option to just toggle LED, or do other work
// Comment this out to do other work.
// #define APP_WORK_IS_TOGGLE_LED

void appWork()
{
#ifdef APP_WORK_IS_TOGGLE_LED
    // Toggle LED on P1.0
    P1OUT ^= BIT0;

    // Store P1OUT value in backup memory register
    *(unsigned int *)BKMEM_BASE = P1OUT;
#else
    WorkRateFSM::step();
#endif
}

void
appColdstart()
{
#ifdef APP_WORK_IS_TOGGLE_LED
    // Clear a backup memory location
    *(unsigned int *)BKMEM_BASE = 0;

    // Store P1OUT value in backup memory register before enter LPM3.5
    // Assert the value is 0
    *(unsigned int *)BKMEM_BASE = P1OUT;
#else
    // On coldstart, no work.
    // Instead, just enter LPM3.5 and wait for more energy.
#endif
}



int main(void)
{
    // Regardless of how we wake up, 
    // stop the WDT and configure GPIO before doing anything else.
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    initGpio();                             // Configure GPIO


    // First determine whether we are coming out of an LPMx.5 or a regular RESET.
    if (SYSRSTIV == SYSRSTIV_LPM5WU)        // When woken up from LPM3.5, reinit
    {
        // MCU wakes up from LPM3.5
        
        // re-init GPIO.  RTC is still powered and does not need configuration.
        
        // Disable the GPIO power-on default high-impedance mode
        // to activate previously configured port settings
        PM5CTL0 &= ~LOCKLPM5;


        // Restore P1OUT value from backup RAM memory, keep P1OUT after LPMx.5 reset
        P1OUT = *(unsigned int *)BKMEM_BASE;

        __enable_interrupt();               
        // The RTC interrupt should trigger now.
        // The ISR does NOT toggle the LED

        appWork();

        // enter LPM3.5 again.
    }
    else
    {
        // Device powered up from a cold start.

        // activate GPIO configuration
        PM5CTL0 &= ~LOCKLPM5;

        // Configure RTC to interrupt every 3 seconds.
        // Only done once, on coldstart.
        // The RTC will request and use the VLO, which stays on.
        initRTC();
        // GIE is off: no interrupts yet

        appColdstart();
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
