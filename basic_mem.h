#pragma once

#include "core_concepts.h"
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
    auto Read8(Address address) noexcept -> u8
    {
        if (address != TTY_STATUS) { return mem_[address]; }
        return 1;
    }

    auto Read16(Address address) noexcept -> u16
    {
        auto* p = reinterpret_cast<u16*>(&mem_[address]);
        return *p;
    }

    auto Read32(Address address) noexcept -> u32
    {
        auto* p = reinterpret_cast<u32*>(&mem_[address]);
        return *p;
    }

    auto Write8(Address address, u8 byte) noexcept -> void
    {
        if (address == TTY_DATA) { std::cout << static_cast<char>(byte); }
        else { mem_[address] = byte; }
    }

    auto Write16(Address address, u16 halfWord) noexcept -> void
    {
        auto* p = reinterpret_cast<u16*>(&mem_[address]);
        *p = halfWord;
    }

    auto Write32(Address address, u32 word) noexcept -> void
    {
        auto* p = reinterpret_cast<u32*>(&mem_[address]);
        *p = word;
    }
};

// A mixin implementation of simple, checked memory that returns std::unexpected on bad access.
class MBasicCheckedMem
{
    // 32KiB of memory.
    std::array<u8, 32768> mem_{};

    // Memory mapped I/O.
    const Address TTY_STATUS = 0x8000;
    const Address TTY_DATA = 0x8001;

public:
    auto Read8(Address address) noexcept -> MemoryResult<u8>
    {
        if (address < mem_.size()) { return mem_[address]; }
        else if (address == TTY_STATUS) { return 1; }
        return std::unexpected{address};
    }

    auto Read16(Address address) noexcept -> MemoryResult<u16>
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            return *p;
        }
        return std::unexpected{address};
    }

    auto Read32(Address address) noexcept -> MemoryResult<u32>
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            return *p;
        }
        return std::unexpected{address};
    }

    auto Write8(Address address, u8 byte) noexcept -> MemoryResult<void>
    {
        if (address < mem_.size())
        {
            mem_[address] = byte;
            return {};
        }
        else if (address == TTY_STATUS)
        {
            std::cout << static_cast<char>(byte);
            return {};
        }
        return std::unexpected{address};
    }

    auto Write16(Address address, u16 halfWord) noexcept -> MemoryResult<void>
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            *p = halfWord;
            return {};
        }
        return std::unexpected{address};
    }

    auto Write32(Address address, u32 word) noexcept -> MemoryResult<void>
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            *p = word;
            return {};
        }
        return std::unexpected{address};
    }
};

class MemoryException : public std::runtime_error
{
private:
    Address address_;

public:
    explicit MemoryException(Address address) : std::runtime_error("Memory error"), address_{address} {}
    Address GetAddress() const { return address_; }
};

// A mixin implementation of simple, checked memory that throws on bad access.
class MBasicThrowingMem
{
    // 32KiB of memory.
    std::array<u8, 32768> mem_{};

    // Memory mapped I/O.
    const Address TTY_STATUS = 0x8000;
    const Address TTY_DATA = 0x8001;

public:
    auto Read8(Address address) -> u8
    {
        if (address < mem_.size()) { return mem_[address]; }
        else if (address == TTY_STATUS) { return 1; }
        throw MemoryException{address};
    }

    auto Read16(Address address) -> u16
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            return *p;
        }
        throw MemoryException{address};
    }

    auto Read32(Address address) -> u32
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            return *p;
        }
        throw MemoryException{address};
    }

    auto Write8(Address address, u8 byte) -> void
    {
        if (address < mem_.size())
        {
            mem_[address] = byte;
        }
        else if (address == TTY_STATUS)
        {
            std::cout << static_cast<char>(byte);
        }
        throw MemoryException{address};
    }

    auto Write16(Address address, u16 halfWord) -> void
    {
        if (address < mem_.size() - 1)
        {
            auto* p = reinterpret_cast<u16*>(&mem_[address]);
            *p = halfWord;
        }
        throw MemoryException{address};
    }

    auto Write32(Address address, u32 word) -> void
    {
        if (address < mem_.size() - 3)
        {
            auto* p = reinterpret_cast<u32*>(&mem_[address]);
            *p = word;
        }
        throw MemoryException{address};
    }
};
