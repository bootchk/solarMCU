#pragma once

#include <cstdint>


class Motor{
public:
    static void turnOn(uint16_t motorDutyCycle);
    static void turnOff(void);
    static void driveAFewRevsAtDutyCycle(uint16_t motorDutyCycle);
    static void driveAFewRevs(void);
};
