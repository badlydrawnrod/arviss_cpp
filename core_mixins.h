// Building blocks that satisfy core concepts for a CPU core.

#pragma once

#include "core_concepts.h"
#include "types.h"

#include <array>

// A mixin implementation of RV32i's integer registers.
// Satisfies: HasXRegisters
class MXRegisters
{
    std::array<u32, 32> xreg_{};

public:
    auto Rx(Reg rs) -> u32 { return xreg_[rs]; }

    auto Wx(Reg rd, u32 val) -> void
    {
        xreg_[rd] = val;
        xreg_[0] = 0;
    }
};

// A mixin implementation of the fetch cycle for the given memory implementation.
// Satisfies: HasFetch, HasMemory
template<HasMemory Mem>
class MFetch : public Mem
{
    Address pc_{};
    Address nextPc_{};

public:
    auto Pc() -> Address const { return pc_; }

    auto Transfer() -> Address
    {
        pc_ = nextPc_;
        return pc_;
    }

    auto Fetch() -> u32
    {
        auto pc = Transfer();
        auto ins = Fetch32(pc);
        SetNextPc(pc + 4);
        return ins;
    }

    auto SetNextPc(Address address) -> void { nextPc_ = address; }

    auto Fetch32(Address address) -> u32
    {
        auto& mem_ = static_cast<Mem&>(*this);
        return mem_.Read32(address);
    }
};

// A mixin implementation of an integer CPU.
// Satisfies: HasXRegisters, HasFetch, HasMemory, IsIntCpu
template<HasMemory Mem>
struct MIntCpu : public MXRegisters, public MFetch<Mem>
{
};
