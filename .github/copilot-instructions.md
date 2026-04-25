# Copilot instructions for solarMCU

## Project purpose and architecture
- This is firmware for an MSP430-class MCU powered directly from a small solar cell + storage capacitor (unregulated Vcc).
- The primary runtime path is [src/main.cpp](../src/main.cpp): wake from low power, decide if energy is sufficient, do brief work, sleep again.
- Core flow is: `main()` -> `WorkRateFSM::step()` -> `Energy::isEnoughToWork()` -> `Work::doWork()` -> `Motor::driveAFewRevs()`.
- `WorkRateFSM` is currently a placeholder policy layer (no persistent FSM state yet) in [src/workRateFSM.cpp](../src/workRateFSM.cpp).

## File roles you should preserve
- [src/main.cpp](../src/main.cpp): preferred app entry with abstractions (`LPM5`, `ResetReason`, `SoC`, `AllPins`).
- [src/ISR.cpp](../src/ISR.cpp): wakeup ISRs (WDT for LPM3, RTC for LPM3.5). Keep ISR bodies minimal.
- [src/app.h](../src/app.h): compile-time app configuration (motor type, pulse length, inter-work period, Vcc threshold).
- [src/energy.cpp](../src/energy.cpp): energy gate based on ADC-measured Vcc in centivolts.
- [src/motor.cpp](../src/motor.cpp): PWM duty-cycle schedule scaled to measured Vcc.
- [src/main2.cpp](../src/main2.cpp): lower-level reference implementation of LPM3.5 flow.
- [src/main3.cpp](../src/main3.cpp): power/current test program, not main app logic.

## Project-specific coding patterns
- Use static utility-style classes (`Class::method`) instead of instances/dynamic allocation.
- Keep energy decisions simple and explicit; thresholds are compile-time macros (`AppMinVccToWork`).
- Preserve centivolt units when reading/propagating Vcc (`ADC::measureVccCentiVolts()`).
- Prefer compile-time feature switches in [src/app.h](../src/app.h) (`AppWorkIsMotor`, `AppMotorIsDC1_3`, `USE_LPM3`) over runtime config.
- Do not add heavy processing inside ISRs; ISRs mostly clear flags and return.

## Low-power and reset behavior (critical)
- LPM3.5 wake is a reset, not a normal function return. Code must branch on reset reason (`ResetReason::isResetAWakeFromSleep()`).
- GPIO low-power configuration must be re-applied on both cold start and wake-reset paths before unlocking LPM5.
- RTC wake source is configured once on cold start in [src/main.cpp](../src/main.cpp); wake path should avoid reconfiguration.
- Interrupt-enable timing is intentional: in wake path, enable interrupts after unlocking configuration so pending RTC interrupt can clear correctly.

## External dependencies and integration
- This repo depends on local `msp430Drivers` headers (e.g., `msp430Drivers/src/...`) and TI DriverLib.
- Include paths assume those dependency trees are available to the toolchain; keep include style consistent.
- No build/test scripts are committed here; treat build/debug as IDE/toolchain-managed embedded workflows.

## Guidance for edits
- When changing behavior, update comments that explain power/energy assumptions (many are design-significant).
- Keep work pulses short and bounded; avoid introducing long active loops that defeat low-power strategy.
- If adding new work types, keep the `Energy` gate before load-driving actions.
- Prefer modifying [src/main.cpp](../src/main.cpp) path unless user explicitly targets test/reference mains.
