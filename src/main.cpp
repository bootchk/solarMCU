
/*
This main() has more explanation
and uses more abstractions.
BUT DOESN'T WORK YET.

It uses LPM3.5 explicitly.  That is not abstracted away.
It hides implementation of LPM3.5 in class LPM5
*/

#include "msp430.h"

// msp430Drivers library

// Hides implemention of LPM3.5
#include "msp430Drivers/src/LPM5/lpm5.h"
#include "msp430Drivers/src/resetReason/resetReason.h"

// Watchdog
#include "msp430Drivers/src/SoC/SoC.h"

// Init GPIO
#include "msp430Drivers/src/pinFunction/allPins.h"

// The app
#include "workRateFSM.h"
#include "app.h"  // configuration of app

//#include "msp430Drivers/src/timer/timer.h"
//#include "msp430Drivers/src/PMM/powerMgtModule.h"
//#include "msp430Drivers/src/PWM/PWM.h"
//#include "msp430Drivers/src/pinFunction/pwmPins.h"
//#include "msp430Drivers/src/timer/counter.h"

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
  /*
  Never returns, when using LPM3.5
  Continuation is a reset, and effectively appWakeFromSleep()
  */
#endif
}
  

/*
When the app needs any extra processing at coldstart,
we do it here.

It should be a minimum,
since energy may be low.
On coldstart, we sleep first, to let energy accumulate.
This accomodates a slowly rising Vcc.
Otherwise, attempting work might brownout.
*/
static void
appColdstart(void)
{
  // Nothing
}

/*
What the app does after a wakeup.
*/
static void
appWakeFromSleep(void)
{
  // Possibly do work here, depending on state machine
  WorkRateFSM::step();
}

#ifdef OLD
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
#endif


int
main (void)
{
  // We don't watchdog to catch abnormal operation.
  // Must be done within 32ms of reset, else watchdog will fire
  SoC::stopWatchDog();

  // Assert the system clock is 1Mhz on reset

  /*
  Read and clear the reset reason device control register
  to know whether reset is:
    coldstart
    or abnormal (ignored and treated as coldstart)
    or an expected reset from LMP3.5
  */
  if (ResetReason::isResetAWakeFromSleep())
  {
    /* 
    Must configure GPIO pins and modules
    exactly as configured for coldstart.
    */
    initGPIOLowPower();
    // Not need to configure RTC again.

    // assert LPM5 lock bit is on
    // assert GIE interrupt not enabled

    // Unlock the LPM5 lock bit
    LPM5::unlockConfiguration();

    /*
    Only now let the wakeup/RTC flag cause an interrupt.
    It will be serviced immediately, and continue after this.

    This is the crux of the dance wierdness:
    the interrupt is serviced after we wakeup.
    We also enable interrupt BEFORE we sleep.
    */
    __enable_interrupt();
    // The ISR has executed but done nothing but clear a flag.

    /*
    The RTC keeps counting and may flag again.
    But the ISR does nothing, and since we are not sleeping,
    the interrupt has no real effect.
    
    When you don't want the RTC to flag and interrupt again,
    you can disable interrupts,
    or unconfigure the RTC.
    LPM5::disableWakeupInterrupt();
    */
    
    // Do the periodic work of the app
    appWakeFromSleep();

    /*
    Require RTC still configured to wake us.
    Sleep again, enter LPM3.5 with interrupt enabled.
    */ 
    appInterWorkPeriod();
  }
  else
  {
    // Coldstart, power on reset

    initGPIOLowPower();
    // Assert the GPIO configuration is same as for wakeup

    // Unlock the LPM5 lock bit
    LPM5::unlockConfiguration();

    // The RTC stays powered in LPM3.5 and needs configuration
    // only once, at coldstart.
    LPM5::configureRTCToWakeup();
    // Assert RTC will flag an interrupt soon, but GIE is not set.
    
    // Do any coldstart specific processing the app wants
    appColdstart();

    // Sleep, enter LPM3.5 for the first time.
    appInterWorkPeriod();
  }
}
  
