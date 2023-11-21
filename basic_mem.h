#pragma once

#include "types.h"

#include <array>
#include <iostream>

// TYPES: concrete mixin. Has own state.

// A mixin implementation of simple, unchecked memory.
class MBasicMem
{
    // 32KiB of memory.
    std::array<u8, 32768> mem_{};

    // Memory mapped I/O.
    const Address TTY_STATUS = 0x8000;
    const Address TTY_DATA = 0x8001;

public:
    auto Read8(Address address) -> u8
    {
        if (address != TTY_STATUS) { return mem_[address]; }
        return 1;
    }

    auto Read16(Address address) -> u16
    {
        auto* p = reinterpret_cast<u16*>(&mem_[address]);
        return *p;
    }

    auto Read32(Address address) -> u32
    {
        auto* p = reinterpret_cast<u32*>(&mem_[address]);
        return *p;
    }

    auto Write8(Address address, u8 byte) -> void
    {
        if (address == TTY_DATA) { std::cout << static_cast<char>(byte); }
        else { mem_[address] = byte; }
    }

    auto Write16(Address address, u16 halfWord) -> void
    {
        auto* p = reinterpret_cast<u16*>(&mem_[address]);
        *p = halfWord;
    }

    auto Write32(Address address, u32 word) -> void
    {
        auto* p = reinterpret_cast<u32*>(&mem_[address]);
        *p = word;
    }
};
