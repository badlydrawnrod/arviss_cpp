// Building blocks that satisfy core concepts for a CPU core.

#pragma once

#include "core_concepts.h"
#include "types.h"

#include <array>
#include <optional>

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

    auto Fetch() -> WordResult
    {
        auto pc = Transfer();
        auto ins = Fetch32(pc);
        if (ins)
        {
            SetNextPc(pc + 4);
        }
        return ins;
    }

    auto SetNextPc(Address address) -> void { nextPc_ = address; }

    auto Fetch32(Address address) -> WordResult
    {
        auto& mem_ = static_cast<Mem&>(*this);
        return mem_.Read32(address);
    }
};

// A mixin implementation of a trap handler.
// Satisfies: HasTraps.
class MTrap
{
    std::optional<TrapState> trap_{};

public:
    auto IsTrapped() const -> bool { return trap_.has_value(); }

    auto TrapCause() const -> std::optional<TrapState> { return trap_; }

    auto RaiseTrap(TrapType type, u32 context = 0) { trap_ = {.type_ = type, .context_ = context}; }
};

// A mixin implementation of an integer core.
// Satisfies: HasTraps, HasXRegisters, HasFetch, HasMemory, IsIntegerCore
template<HasMemory Mem>
struct MIntegerCore : public MTrap, public MXRegisters, public MFetch<Mem>
{
};
