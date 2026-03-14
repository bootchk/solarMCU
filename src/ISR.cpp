
#include <msp430.h>



/*
ISR for the WDT interrupt
We use the WDT as an interval timer only.
For delays in LPM3.
See mspdrivers/lowPower/lowPowerTimer.h

!!! the lowPowerTimer must use the WDT, not the RTC used for other purposes.
*/
#pragma vector = WDT_VECTOR
__interrupt void WDT_ISR(void)
{
    // Nothing to do, just wake from LPM3
    // We don't set any flags to record the purpose of the timer lapse.
    // Main just continues after any delay.
    //
    // There can be many reasons for delay:
    // 1. for sleeping period.
    // 2. delay during work (say blink LED).

    // Exit LPM3 on return (RETI)to main
    // This clears the LPM3 bits in the SR saved on the stack.
    __bic_SR_register_on_exit(LPM3_bits);
}


/* 
ISR for the RTC device.
We expect we are waking from LPM3.5.
*/
#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{
  /* 
  Must read RTCIV to clear the flag.
  Otherwise, will interrupt again.

  We don't care whether is an interrupt pending
  (can be None 0, or overflow RTGIFG)
  */
  switch(RTCIV)
    {
        case RTCIV_NONE :  // No interrupt pending
        case RTCIV_RTCIF:  // RTC Overflow
        default:         
          break;
    }

  // No need to clear LPM3?
  //__bic_SR_register_on_exit(LPM3_bits);
}



/*
Catch unintended interrupts.
*/

#pragma vector=unused_interrupts
interrupt void myTrap(void)
{
  while (true)
  {
    /*
     * Break into the debugger (aka BKPT) if it is running, else NOP.
     * The EEM on MSP430 looks for this opcode.
     */
    __op_code(0x4343);
  }
}
