#pragma once

#include <cstdint>
#include <exception>

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

struct TrapState
{
    TrapType type_;
    u32 context_;
};

class TrappedException : public std::exception
{
    TrapState cause_;

public:
    explicit TrappedException(TrapType type) : cause_{.type_ = type, .context_ = 0} {}
    TrapType Reason() const { return cause_.type_; }
    u32 Context() const { return cause_.context_; }
};
