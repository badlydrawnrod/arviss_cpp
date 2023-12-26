#pragma once

#include "arviss/common/types.h"

#include <iostream>
#include <vector>

namespace arviss
{
    // A mixin implementation of a simple, checked address space that can signal bad access. It also has some simple
    // memory-mapped I/O in the form of a TTY.
    class BasicMem
    {
        // 32KiB of memory.
        std::vector<u8> mem_ = std::vector<u8>(32768);

        // Memory mapped I/O.
        const Address TTY_STATUS = 0x8000;
        const Address TTY_DATA = 0x8001;

    public:
        auto Read8(Address address) -> u8
        {
            // TODO: Given that this is *checked* memory, then we could use mem_.at(address) and re-throw std::out_of_range as a TrappedException.
            // And, yes, this is true, but the underlying code looks no better - in fact, slightly worse because it throws one exception only to
            // call another.
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
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
                auto* p = reinterpret_cast<u16*>(&mem_[address]);
                return *p;
            }
            throw TrappedException(TrapType::LoadAccessFault);
        }

        auto Read32(Address address) -> u32
        {
            if (address < mem_.size() - 3)
            {
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
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
                std::cout << static_cast<char>(byte);
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
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
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
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
                auto* p = reinterpret_cast<u32*>(&mem_[address]);
                *p = word;
            }
            else
            {
                throw TrappedException(TrapType::StoreAccessFault);
            }
        }
    };

    // A mixin implementation of a simple, checked address space that can signal bad access. It has a null
    // implementation of memory mapped I/O for benchmarking purposes.
    class NoIoMem
    {
        // 32KiB of memory.
        std::vector<u8> mem_ = std::vector<u8>(32768);

        // Memory mapped I/O.
        const Address TTY_STATUS = 0x8000;
        const Address TTY_DATA = 0x8001;

    public:
        auto Read8(Address address) -> u8
        {
            // TODO: Given that this is *checked* memory, then we could use mem_.at(address) and re-throw std::out_of_range as a TrappedException.
            // And, yes, this is true, but the underlying code looks no better - in fact, slightly worse because it throws one exception only to
            // call another.
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
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
                auto* p = reinterpret_cast<u16*>(&mem_[address]);
                return *p;
            }
            throw TrappedException(TrapType::LoadAccessFault);
        }

        auto Read32(Address address) -> u32
        {
            if (address < mem_.size() - 3)
            {
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
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
                // Do nothing.
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
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
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
                // TODO: What if we're on a big-endian machine, or one that complains about alignment?
                auto* p = reinterpret_cast<u32*>(&mem_[address]);
                *p = word;
            }
            else
            {
                throw TrappedException(TrapType::StoreAccessFault);
            }
        }
    };

} // namespace arviss
