 
#include "energy.h"

#include "app.h"



#if EnergyFromVcc

// implementation uses ADC on Vcc
#include "msp430Drivers/src/ADC/adc.h"


void Energy::initPin() {
        // No pin to init for this implementation.
}

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

#elif defined(EnergyFromVHighRail)

// Implementation uses GPIO
#include <gpio.h>   // depends on msp430.h

#define VoltageMonitorPort  GPIO_PORT_P1
#define VoltageMonitorPin   GPIO_PIN0

void Energy::initPin() {
        // voltage monitor pin is input with no pullup
        GPIO_setAsInputPin(VoltageMonitorPort, VoltageMonitorPin);
}


/*
Check if there is enough energy to perform work.
Energy is from another voltage rail.
That has a voltage monitor with digital out that is high
when voltage is above some threshold.
*/
bool Energy::isEnoughToWork() {
        // When voltage monitor pin is high
        return (GPIO_getInputPinValue(VoltageMonitorPort, VoltageMonitorPin) != 0);
};


#else
    #error "Set EnergyFromVcc or EnergyFromVHighRail"
#endif
