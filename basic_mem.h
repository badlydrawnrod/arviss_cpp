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
    auto Read8(Address address) -> ByteResult
    {
        if (address < mem_.size())
        {
            return mem_[address];
        }
        else if (address == TTY_STATUS)
        {
            return 1;
        }
        return std::unexpected(MemoryError::BadLoad);
    }

    auto Read16(Address address) -> HalfwordResult
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            return *p;
        }
        return std::unexpected(MemoryError::BadLoad);
    }

    auto Read32(Address address) -> WordResult
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            return *p;
        }
        return std::unexpected(MemoryError::BadLoad);
    }

    auto Write8(Address address, u8 byte) -> WriteResult
    {
        if (address < mem_.size())
        {
            mem_[address] = byte;
            return {};
        }
        else if (address == TTY_DATA)
        {
            std::cout << static_cast<char>(byte);
            std::flush(std::cout);
            return {};
        }
        return std::unexpected(MemoryError::BadStore);
    }

    auto Write16(Address address, u16 halfWord) -> WriteResult
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            *p = halfWord;
            return {};
        }
        return std::unexpected(MemoryError::BadStore);
    }

    auto Write32(Address address, u32 word) -> WriteResult
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            *p = word;
            return {};
        }
        return std::unexpected(MemoryError::BadStore);
    }
};
