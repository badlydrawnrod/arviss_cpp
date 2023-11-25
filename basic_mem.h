#pragma once

#include "types.h"

#include <array>
#include <iostream>

// TYPES: concrete mixin. Has own state.

// A mixin implementation of simple, checked memory that can signal bad access.
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
        if (address < mem_.size())
        {
            return mem_[address];
        }
        else if (address == TTY_STATUS)
        {
            return 1;
        }
        throw TrappedException(TrapType::LoadAccessFault);
    }

    auto Read16(Address address) -> u16
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            return *p;
        }
        throw TrappedException(TrapType::LoadAccessFault);
    }

    auto Read32(Address address) -> u32
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            return *p;
        }
        throw TrappedException(TrapType::LoadAccessFault);
    }

    auto Write8(Address address, u8 byte) -> void
    {
        if (address < mem_.size())
        {
            mem_[address] = byte;
        }
        else if (address == TTY_DATA)
        {
            // std::cout << static_cast<char>(byte);
            // std::flush(std::cout);
        }
        else
        {
            throw TrappedException(TrapType::StoreAccessFault);
        }
    }

    auto Write16(Address address, u16 halfWord) -> void
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            *p = halfWord;
        }
        else
        {
            throw TrappedException(TrapType::StoreAccessFault);
        }
    }

    auto Write32(Address address, u32 word) -> void
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            *p = word;
        }
        else
        {
            throw TrappedException(TrapType::StoreAccessFault);
        }
    }
};
