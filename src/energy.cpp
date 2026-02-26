 
#include "energy.h"

#include "app.h"

#include "msp430Drivers/src/ADC/adc.h"

/*
Check if there is enough energy to perform work.
Energy is from a capacitor at Vcc charged by a solar cell.
If Vcc is greater than some threshold, capacitor has enough energy to work,
without Vcc dropping below the brownout threshold during work.

If this threshold is not high enough,
system will brownout reset.
That sets the load enable pin to its reset condition
which will turn off the load, prematurely.
The pin should have an external pulldown
ensure that a high impedance condition from the MCU
is seen as a low by the load driver.

Even if brownout, no two works will be less than a period apart.
The brownout just means that the work is partial, 
i.e. failed to complete.
*/
bool Energy::isEnoughToWork() {
        // more than 2.2V
        return  (ADC::measureVccCentiVolts() > AppMinVccToWork);
};
