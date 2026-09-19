
#include "app.h"
#include "appConfig.h"



#ifdef AppWorkIsLED

void App::work()
{
    // Toggle LED on P1.0
    P1OUT ^= BIT0;

    // Store P1OUT value in backup memory register
    //*(unsigned int *)BKMEM_BASE = P1OUT;
}

void
AppC::coldStart()
{
    // Clear a backup memory location
    *(unsigned int *)BKMEM_BASE = 0;

    // Store P1OUT value in backup memory register before enter LPM3.5
    // Assert the value is 0
    *(unsigned int *)BKMEM_BASE = P1OUT;
}


#elif defined(AppWorkIsMotor)

// App is driven by a FSM

#include "workRateFSM.h"

void App::work()
{
    WorkRateFSM::step();
}

void
App::coldStart()
{
    /*
    On coldstart, init work FSM.
    Reset does not initialize FRAM and state variables.
    */ 
    WorkRateFSM::init();
    // Now enter LPM3.5 and wait for more energy.
}

#else
    #error "Define AppWorkIs..."
#endif
