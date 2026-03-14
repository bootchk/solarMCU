/* This main is just a test.
Enters LPM3 and stays there.
For testing current.
*/

#include <msp430.h>
#include <gpio.h>
#include <wdt_a.h>
#include <pmm.h>

#include <cs.h>

void main (void)
{
    WDT_A_hold(WDT_A_BASE);

    GPIO_setOutputLowOnPin(GPIO_PORT_P1, 0xFF);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, 0xFF);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3, 0xFF);

    // Set all pins to output mode to save power
    GPIO_setAsOutputPin(GPIO_PORT_P1, 0xFF);
    GPIO_setAsOutputPin(GPIO_PORT_P2, 0xFF);
    GPIO_setAsOutputPin(GPIO_PORT_P3, 0xFF);

    PMM_unlockLPM5();

    // Let REFO turn off
    CS_initClockSignal(CS_MCLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);

    //Enter LPM3 mode
    __bis_SR_register(LPM3_bits);
    __no_operation();
}