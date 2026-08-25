#include "workRateFSM.h"

#include "work.h"
#include "energy.h"

// A simple finite state machine that keeps track of how much work
// we are doing, based on energy availability.
// The step function is called every waking period.
// Update the state to show whether we are working none, some, or all recent periods.
// IOW, the FSM knows whether we worked in this and the last period.


/*
Turn the motor.
Then enough wait states for the system to return to stopped condition.
Here, wait 5 periods (minutes.)
*/

enum class FSMState : unsigned char {
    Start,
    Turned,
    Wait1,
    Wait2,
    Wait3,
    Wait4,
};


#pragma PERSISTENT
static FSMState workState = FSMState::Start;

/* Reset the work FSM.
The physical system driven by the motor
could be in an unknown state.
We should reset to Turned and wait for physical system to stabilize.
But that takes say 5 minutes.
Assume that a reset comes from energy exhaustion below MCU Vmin (1.9V)
and that the physical system stabilized while energy built back up.
So reset to Start.
*/
void 
WorkRateFSM::init (void) {
  workState = FSMState::Start;
}

void 
WorkRateFSM::step (void) {
  
  switch (workState)
  {
    case FSMState::Start:
      if (Energy::isEnoughToWork()) {
        if (Work::doWork()) {
          // Positive feedback that motor turned
          workState = FSMState::Turned;
        }
        else {
          // Not certain the motor actually turned,
          // but it might have.  Assume it did.
          // We always wait, i.e. allow time to unwind.
          workState = FSMState::Turned;
        }
      }
      else {
        /* Not enough energy.
        Physical system is unwound, and we should turn but can't.
        workState remains Start.
        */
        Work::doNotWork();  // Do something requiring low energy?
      }
      break;

    /*
    Advance through wait states.
    We wake every AppInterWakePeriod, say 1 minute.
    Wait long enough for physical system to unwind, say 5 minutes.
    */
    /* To shortcut, temporarily change one transtion back to Start. */
    case FSMState::Turned:
      workState = FSMState::Wait1;
      break;
    case FSMState::Wait1:
      workState = FSMState::Wait2;
      break;
    case FSMState::Wait2:
      workState = FSMState::Wait3;
      break;
    case FSMState::Wait3:
      workState = FSMState::Wait4;
      break;
    case FSMState::Wait4:
      workState = FSMState::Start;
      break;

    default:
      // Defensive recovery in case workState is corrupted.
      workState = FSMState::Start;
      break;
  }
};

