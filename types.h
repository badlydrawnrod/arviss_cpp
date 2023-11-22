#pragma once

#include <cstdint>
#include <expected>

// TYPES: All global to the entire shebang.

using Address = std::uint32_t;
using Reg = std::uint32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using f32 = float;

enum class TrapType
{
    // Non-interrupt traps.
    InstructionAddressMisaligned,
    InstructionAccessFault,
    IllegalInstruction, // (u32)
    Breakpoint,
    LoadAddressMisaligned,
    LoadAccessFault, // (Address)
    StoreAddressMisaligned,
    StoreAccessFault, // (Address)
    EnvironmentCallFromUMode,
    EnvironmentCallFromSMode,
    EnvironmentCallFromMMode,
    InstructionPageFault,
    LoadPageFault,
    StorePageFault,
    // Interrupts.
    SupervisorSoftwareInterrupt,
    MachineSoftwareInterrupt,
    SupervisorTimerInterrupt,
    MachineTimerInterrupt,
    SupervisorExternalInterrupt,
    MachineExternalInterrupt,
};

template<typename T>
using MemoryResult = std::expected<T, Address>;
