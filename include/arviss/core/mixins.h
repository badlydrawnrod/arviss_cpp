// Building blocks that satisfy core concepts for a CPU core.

#pragma once

#include "arviss/core/concepts.h"
#include "arviss/common/types.h"

#include <array>
#include <optional>

namespace arviss
{
    // A mixin implementation of RV32i's integer registers.
    // Satisfies: HasXRegisters
    class XRegisters
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

    // A mixin implementation of RV32f's float registers.
    // Satisfies: HasFRegisters
    class FRegisters
    {
        std::array<f32, 32> freg_{};

    public:
        auto Rf(Reg rs) -> f32 { return freg_[rs]; }

        auto Wf(Reg rd, f32 val) -> void { freg_[rd] = val; }
    };

    // A mixin implementation of the fetch cycle for the given memory implementation. BYO memory.
    // Satisfies: HasFetch, HasMemory
    template<HasMemory Mem>
    class Fetcher : public Mem
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
            if ((ins & 0b11) == 0b11)
            {
                // 32-bit instruction.
                SetNextPc(pc + 4);
            }
            else
            {
                // 16-bit compressed instruction.
                SetNextPc(pc + 2);
                ins = ins & 0xffff;
            }
            return ins;
        }

        auto SetNextPc(Address address) -> void { nextPc_ = address; }

        auto Fetch32(Address address) -> u32
        {
            auto& mem_ = static_cast<Mem&>(*this);
            return mem_.Read32(address);
        }
    };

    // A mixin implementation of a trap handler.
    // Satisfies: HasTraps.
    class TrapHandler
    {
        std::optional<TrapState> trap_{};

    public:
        auto IsTrapped() const -> bool { return trap_.has_value(); }
        auto TrapCause() const -> std::optional<TrapState> { return trap_; }
        auto RaiseTrap(TrapType type, u32 context = 0) { trap_ = {.type_ = type, .context_ = context}; }
        auto ClearTraps() { trap_ = {}; }
    };

    // A mixin implementation of an integer core. BYO memory.
    // Satisfies: IsIntegerCore (HasTraps, HasXRegisters, HasFetch, HasMemory)
    template<HasMemory Mem>
    struct IntegerCore : public TrapHandler, public XRegisters, public Fetcher<Mem>
    {
    };

    // A mixin implementation of a floating point core. BYO memory.
    // Satisfies: IsFloatCore (IsIntegerCore + HasFRegisters)
    template<HasMemory Mem>
    struct FloatCore : public TrapHandler, public XRegisters, public FRegisters, public Fetcher<Mem>
    {
    };

} // namespace arviss
