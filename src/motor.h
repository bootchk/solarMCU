#pragma once

#include <cstdint>


class Motor{
public:
    static void turnOn(uint16_t motorDutyCycle);
    static void turnOff(void);
    
    static bool driveAFewRevs(void);
};
