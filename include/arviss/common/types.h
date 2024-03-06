#pragma once

#include <concepts>
#include <cstdint>
#include <exception>
#include <optional>

namespace arviss
{
    using Address = std::uint32_t;
    using Reg = std::uint32_t;
    using i8 = std::int8_t;
    using u8 = std::uint8_t;
    using i16 = std::int16_t;
    using u16 = std::uint16_t;
    using i32 = std::int32_t;
    using u32 = std::uint32_t;
    using i64 = std::int64_t;
    using u64 = std::uint64_t;
    using f32 = float;
    using f64 = double;

    enum RegNames
    {
        ZERO = 0,
        RA = 1,
        SP = 2
    };

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

    // T is an instruction dispatcher.
    template<typename T>
    concept IsDispatcher = requires(T t) {
        {
            t.Dispatch(u32{})
        } -> std::same_as<typename T::Item>;
    };

    // T supports reading from and writing to integer registers.
    template<typename T>
    concept HasXRegisters = requires(T t, u32 result) {
        result = t.Rx(Reg{}); // Reads from an integer register.
        t.Wx(Reg{}, u32{});   // Writes to an integer register.
    };

    // T supports reading from and writing to floating point registers.
    template<typename T>
    concept HasFRegisters = requires(T t, f32 result) {
        result = t.Rf(Reg{}); // Reads from a float register.
        t.Wf(Reg{}, f32{});   // Writes to a float register.
    };

    // T supports writing to memory without checking if it's allowed to. The use case for this is being able to write
    // to "ROM" that isn't available to the VM.
    template<typename T>
    concept HasUnprotectedWrites = requires(T t, u8 b, u16 h, u32 w) {
        t.Write8Unprotected(Address{}, u8{});   // Writes a byte to an address, even if it's read-only for the VM.
        t.Write16Unprotected(Address{}, u16{}); // Writes a halfword to an address, even if it's read-only for the VM.
        t.Write32Unprotected(Address{}, u32{}); // Writes a word to an address, even if it's read-only for the VM.
    };

    // T supports reading from and writing to memory.
    template<typename T>
    concept HasMemory = requires(T t, u8 b, u16 h, u32 w) {
        b = t.Read8(Address{});      // Reads a byte from an address.
        h = t.Read16(Address{});     // Reads a halfword from an address.
        w = t.Read32(Address{});     // Reads a word from an address.
        t.Write8(Address{}, u8{});   // Writes a byte to an address.
        t.Write16(Address{}, u16{}); // Writes a halfword to an address.
        t.Write32(Address{}, u32{}); // Writes a word to an address.
    };

    // T implements the fetch cycle.
    template<typename T>
    concept HasFetch = requires(T t, Address a, u32 w) {
        a = t.Pc();               // Returns the contents of the program counter.
        a = t.Transfer();         // Transfers nextPc to the program counter and returns it.
        w = t.Fetch();            // Transfer, Fetch32(pc), SetNextPc, return instruction.
        t.SetNextPc(Address{});   // Sets nextPc.
        w = t.Fetch32(Address{}); // Returns the instruction at the given address.
    };

    // T has traps.
    template<typename T>
    concept HasTraps = requires(T t, TrapType type, u32 context, bool b, std::optional<TrapState> c) {
        b = t.IsTrapped();          // Returns true if a trap is currently active.
        c = t.TrapCause();          // Returns the cause of the trap.
        t.RaiseTrap(type);          // It can raise a trap of the given type.
        t.RaiseTrap(type, context); // It can raise a trap of the given type with the given context.
        t.ClearTraps();             // It can clear traps.
    };

    // T has all the pieces of an integer core.
    template<typename T>
    concept IsIntegerCore = HasTraps<T> // It has traps.
            && HasXRegisters<T>         // It has integer registers.
            && HasFetch<T>              // It has a fetch cycle implementation.
            && HasMemory<T>;            // It has memory.

    // T has all the pieces of a floating point core.
    template<typename T>
    concept IsFloatCore = IsIntegerCore<T> // It's also an integer core
            && HasFRegisters<T>;           // It has floating point registers.

} // namespace arviss
