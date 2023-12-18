// Building blocks that satisfy core concepts for a CPU core.

#pragma once

#include "arviss/common/types.h"
#include "arviss/core/concepts.h"

#include <array>
#include <optional>

namespace arviss
{
    // A do-nothing implementation of memory, for concept checking.
    struct NullMem
    {
        auto Read8(Address) -> u8 { return 0; }
        auto Read16(Address) -> u16 { return 0; }
        auto Read32(Address) -> u32 { return 0; }

        auto Write8(Address, u8) -> void {}
        auto Write16(Address, u16) -> void {}
        auto Write32(Address, u32) -> void {}
    };

    static_assert(HasMemory<NullMem>);

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

    static_assert(HasXRegisters<XRegisters>);

    // A mixin implementation of RV32f's float registers.
    // Satisfies: HasFRegisters
    class FRegisters
    {
        std::array<f32, 32> freg_{};

    public:
        auto Rf(Reg rs) -> f32 { return freg_[rs]; }

        auto Wf(Reg rd, f32 val) -> void { freg_[rd] = val; }
    };

    static_assert(HasFRegisters<FRegisters>);

    // A mixin implementation of the fetch cycle for the given memory implementation. BYO memory.
    // Satisfies: HasFetch, HasMemory
    template<HasMemory Mem>
    class Fetcher : public Mem
    {
        Address pc_{};
        Address nextPc_{};

    public:
        auto Pc() const -> Address { return pc_; }

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
            auto& mem = static_cast<Mem&>(*this);
            return mem.Read32(address);
        }
    };

    static_assert(HasFetch<Fetcher<NullMem>> && HasMemory<Fetcher<NullMem>>);

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

    static_assert(HasTraps<TrapHandler>);

    // A mixin implementation of an integer core. BYO memory.
    // Satisfies: IsIntegerCore (HasTraps, HasXRegisters, HasFetch, HasMemory)
    template<HasMemory Mem>
    struct IntegerCore : public TrapHandler, public XRegisters, public Fetcher<Mem>
    {
    };

    static_assert(IsIntegerCore<IntegerCore<NullMem>>);

    // A mixin implementation of a floating point core. BYO memory.
    // Satisfies: IsFloatCore (IsIntegerCore + HasFRegisters)
    template<HasMemory Mem>
    struct FloatCore : public TrapHandler, public XRegisters, public FRegisters, public Fetcher<Mem>
    {
    };

    static_assert(IsFloatCore<FloatCore<NullMem>>);

} // namespace arviss
