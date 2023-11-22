// Concepts that relate to a CPU core.

#pragma once

#include "types.h"

#include <concepts>
#include <optional>
#include <string>

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
concept HasFetch = requires(T t, Address a, u32 r) {
    a = t.Pc();               // Returns the contents of the program counter.
    a = t.Transfer();         // Transfers nextPc to the program counter and returns it.
    r = t.Fetch();            // Transfer, Fetch32(pc), SetNextPc, return instruction.
    t.SetNextPc(Address{});   // Sets nextPc.
    r = t.Fetch32(Address{}); // Returns the instruction at the given address.
};

// T has all the pieces of an integer core.
template<typename T>
concept IsIntegerCore = HasXRegisters<T> // It has integer registers.
        && HasFetch<T>                   // It has a fetch cycle implementation.
        && HasMemory<T>;                 // It has memory.

// T can handle traps.
template<typename T>
concept IsTrapHandler = requires(T t, std::optional<TrapType> optionalCause, TrapType cause, bool b) {
    optionalCause = t.TrapCause(); // Returns the cause of the current trap, or std::nullopt if not trapped (TODO: alternatively, have a "not trapped" cause).
    t.ClearTrap();                 // Clears the current trap cause.
    t.HandleTrap(cause);           // Handles the given trap.
    b = t.IsTrapped();             // Returns true if the core is trapped.
};

// T has special handling for ECALL and EBREAK.
// TODO: is this necessary ... could an instruction handler do it itself?
template<typename T>
concept IsEcallHandler = requires(T t) {
    t.HandleEcall();  // Handles ECALL instructions.
    t.HandleEbreak(); // Handles EBREAK instructions.
};
