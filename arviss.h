#pragma once

#include "concepts.h"
#include "types.h"

#include <array>

// TYPES: building blocks. Can be used, but not mandatory.

// A mixin implementation of RV32i's integer registers.
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
