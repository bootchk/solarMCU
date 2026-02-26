
#include "msp430.h"

// msp430Drivers library
#include "msp430Drivers/src/SoC/SoC.h"
#include "msp430Drivers/src/pinFunction/allPins.h"
#include "msp430Drivers/src/timer/timer.h"
#include "msp430Drivers/src/PMM/powerMgtModule.h"
#include "msp430Drivers/src/PWM/PWM.h"
#include "msp430Drivers/src/pinFunction/pwmPins.h"
#include "msp430Drivers/src/resetReason/resetReason.h"
#include "msp430Drivers/src/timer/counter.h"
#include "msp430Drivers/src/LPM5/lpm5.h"



#include "workRateFSM.h"

#include "app.h"


/*
App periodically wakes from low power mode to attempt work.
The period is "interwork period."
Between interwork periods, the app works if energy is available.

A state machine (FSM) can track how much work we are doing
(whether we are working none, some, or all interwork periods).
The FSM can also use more or less energy depending on state.

Power mode strategy:

Time in active mode is short (mSec) compared to time in LPM (many seconds), 
so average current is low.

Main is all in mode active, except for the interwork periods.
The interwork periods are in low power mode LPM (aka sleep, standby, or off.)
A compile time choice is whether LPM is LPM3, LPM3.5, etc.

WorkRateFSM does not enter low power mode itself.
Work current (say 4mA) is much more than active mcu current (500uA)
so we don't bother to sleep the mcu while doing work.

If the FSM also needs to enter LPM,
that must be coordinated with the LPM of the interwork periods.
E.G. the interrupt handlers must be coordinated.
*/


/*
This main uses LPM3.5 (wake from RTC is a reset.)
On most MSP430 family members, LPM3.5 draws 1uA.

An alternative is LPM3 (wake from WDT is not a reset)
but on some family members when external crystal oscillator not populated,
the REFO still operates and draws 15uA.

The decision to use LPM3.5 affects main() and configuration of msp430Drivers lib.
See the configuration of LowPowerTimer;
it enters LPM3.5.
*/

/*
Configure GPIO pins to low power.
On reset, they are high impedance, indeterminate out.

Later, we configure some GPIO pins for app's use.
*/
void
initGPIOLowPower(void)
{
  // Configure GPIO pins to low power state.
  AllPins::configureGPIOLowPower();

  // Out value is indeterminate after POR reset: set to low
  AllPins::setAllOutputsLow();

  // At power-on, LOCKLPM5 bit defaults to LOCKED.
  // Unlock to activate just configured port settings.
  // PMM::unlockLPM5();
}


/*
Wait doing nothing.

This may be a stop with reset (LPM3.5)
OR a standby (LPM3)

When stop with reset, this does not return.
*/
static void
appInterWorkPeriod(void)
{

#ifdef USE_LPM3
  // Macro to call LowPowerTimer with a predetermined period length.
  AppInterWorkPeriod();
#else
  // LPM3.5
  LPM5::haltWithRTCWakeup();
#endif
}


/* 
On coldstart, we sleep first, to let energy accumulate.
This accomodates a slowly rising Vcc.
Otherwise, attempting work might brownout.

This is NOT an epxplicit while loop,
but does loop through the LPM3.5 and reset mechanism.
*/
static void
appColdstart(void)
{
  // Sleep
  appInterWorkPeriod();
  /*
  Never returns, when using LPM3.5
  Continuation is a reset, and effectively appWakeFromSleep()
  */
}

static void
appWakeFromSleep(void)
{
  // Possibly do work here, depending on state machine
  WorkRateFSM::step();

   appInterWorkPeriod();
  /*
  Never returns, when using LPM3.5
  Continuation is a reset, and eventually appWakeFromSleep() again
  */
}

/* 
This knows the wakeup is RTC aka Counter.

The ISR does not clear the flag,
since the ISR runs with LPM5 bit locked.
*/
static void
clearWakeupInterrupt(void)
{
  Counter::disableAndClearOverflowInterrupt();
}



int
main (void)
{
  // We don't watchdog to catch abnormal operation.
  // Must be done within 32ms of reset, else watchdog will fire
  SoC::stopWatchDog();

  // Assert the system clock is 1Mhz on reset

  /*
  Read and clear the reset reason device control register
  to know whether reset is coldstart (or abnormal)
  or an expected reset from LMP3.5
  */
  if (ResetReason::isResetAWakeFromSleep())
  {
    initGPIOLowPower();
    // assert LPM5 bit is off
    clearWakeupInterrupt();
    // TODO stop Counter?
    appWakeFromSleep();
  }
  else
  {
    // Coldstart, power on reset
    initGPIOLowPower();
    appColdstart();
  }
}
  




/*
ISR for the WDT interrupt
We use the WDT as an interval timer only.
For delays in LPM3.
See mspdrivers/lowPower/lowPowerTimer.h
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



#pragma vector = RTC_VECTOR
__interrupt void RTC_ISR(void)
{
  __bic_SR_register_on_exit(LPM3_bits);
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
