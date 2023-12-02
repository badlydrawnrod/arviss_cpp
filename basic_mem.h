#pragma once

#include "types.h"

#include <iostream>
#include <vector>

namespace arviss
{
    // TODO: make BasicMem go to the heap if it gets above a certain size. We can do this with requirements, and have it backed by std::array or std::vector.
    // And then there's also the possibility of doing some sort of paged memory, or at least memory where programs can share the same kernel even if they have
    // to go through it to get access to the hardware. That being said, there is no actual hardware as this is an entirely soft CPU, so even that's a relative
    // term. Really we just don't want programs to compete for the same resources if simultaneous access to those resources would screw things up. Not a problem
    // if programs can do what they like, but pass messages / make syscalls if they need something to happen. For example, why can't a single program have
    // access to the graphics card (hypothetically) and all other programs have to talk to it to output anything. Ditto sockets, etc. It doesn't have to be the
    // kernel that does it.
    //
    // As for going to the heap above a certain size... we'd probably want to do that for all memory in reality, otherwise we end up with giant structs.

    // A mixin implementation of simple, checked memory that can signal bad access.
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

} // namespace arviss
