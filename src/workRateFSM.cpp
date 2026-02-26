#include "workRateFSM.h"

#include "work.h"
#include "energy.h"

// A simple finite state machine that keeps track of how much work
// we are doing, based on energy availability.
// The step function is called every waking period.
// Update the state to show whether we are working none, some, or all recent periods.
// IOW, the FSM knows whether we worked in this and the last period.

void 
WorkRateFSM::step (void) {
    // Placeholder implementation. Actual state machine would go here
    
    // For now, just work when enough energy is available.
    if (Energy::isEnoughToWork())
      Work::doWork();
    else
      Work::doNotWork();
};
