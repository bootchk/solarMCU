
#include <msp430.h>

#include "msp430Drivers/src/motorControl/motorControl.h"

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

// Pin interrupt from motor drive FG pin
// to GPIO pin as input.
#pragma vector = PORT2_VECTOR
__interrupt void Port2_ISR(void)
{
  MotorControl::handlePinInterrupt();
  // Not in LPM
}


#if MOTOR_CONTROL_IS_COUNTING

// Special vector for CCR0 of TimerB0.
// No need to read TBIV, and clears CCR0IFG automatically
// AKA TIMERB0_VECTOR
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0_CCR0_ISR(void)
{
  MotorControl::handleTimerInterrupt();
  // Not in LPM
}

#endif


#ifdef CRUFT
// Vector for CCR1,2 and overflow of TimerB0.
#pragma vector = TIMER0_B1_VECTOR
__interrupt void TimerB0_Other_ISR(void)
{
  // Reading the interrupt flags clears highest flag?
  switch(TBIV)
    {
        // Cases are TBIV__NONE, TBIV__TBCCR1, TBIV__TBCCR2, TBIV__TBIFG

        // All are unexpected: not using CCR1, CCR2, or overflow
        default:
          // In all cases, pretend correct
          MotorControl::handleInterrupt();
          break;
    }
  // Not in LPM
}
#endif


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
