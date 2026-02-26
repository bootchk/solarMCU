WORK IN PROGRESS


## About

The program for a HW system that solar powers an MCU.

A future document describes the design considerations

## The system

System storage is a small tantalum capacitor, say 4mF.

The MCU is ultra-low power, say an MSP430
(or in the future, an MSPM0.)

Power is from a solar cell matched to the MCU.
A Powerfilm model LL200-2.4-37.
In all light conditions it never produces more voltage
than the Vmax of the MCU.
In the design light condition of 200lux,
it produces a few tens of uA current at voltage no more than 2.7V.

The system does not use MPPT (maxium power point tracking);
the solar cell is not used with high efficiency.

The program sleeps mostly, with the MCU in a low power mode.
Thus the average current draw is about 1uA.

The program wakes occasionally to do work if energy permits.

The HW system voltage Vcc is unregulated.
It varies as light conditions change.

The system is designed for indoor light conditions,
say only 200lux.

## MSP430 low power modes

LPM3 is much easier to program.
LPM3 is "standby": the MCU is ready to resume execution quickly.
But on some members of the MSP430 family,
LPM3 draws 15uA since the REFO oscillator remains active.

LPM3.5 is harder to program.
LPM3.5 is "shutdown": the MCU is not powered and restarts with a reset.
But it only draws 1uA.
It requires a careful dance in main(), as shown here.

## Requirements

Uses msp430drivers repo, which uses TI DriverLib repo.



