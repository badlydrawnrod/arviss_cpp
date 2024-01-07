#pragma once

#include "arviss/common/types.h"

#include <bit>
#include <iostream>
#include <vector>

namespace arviss
{
    // If you're not an a little-endian platform or a big-endian platform then all bets are off.
    static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

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
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u16*>(&mem_[address]);
                    return *p;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    return (mem_[address] << 8) | mem_[address + 1];
                }
            }
            throw TrappedException(TrapType::LoadAccessFault);
        }

        auto Read32(Address address) -> u32
        {
            if (address < mem_.size() - 3)
            {
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u32*>(&mem_[address]);
                    return *p;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    return (mem_[address] << 24) | (mem_[address + 1] << 16) | (mem_[address + 2] << 8) | mem_[address + 3];
                }
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
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u16*>(&mem_[address]);
                    *p = halfWord;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    mem_[address] = (halfWord >> 8) & 0xffu;
                    mem_[address + 1] = halfWord & 0xffu;
                }
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
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u32*>(&mem_[address]);
                    *p = word;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    mem_[address] = (word >> 24) & 0xffu;
                    mem_[address + 1] = (word >> 16) & 0xffu;
                    mem_[address + 2] = (word >> 8) & 0xffu;
                    mem_[address + 3] = word & 0xffu;
                }
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
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u16*>(&mem_[address]);
                    return *p;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    return (mem_[address] << 8) | mem_[address + 1];
                }
            }
            throw TrappedException(TrapType::LoadAccessFault);
        }

        auto Read32(Address address) -> u32
        {
            if (address < mem_.size() - 3)
            {
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u32*>(&mem_[address]);
                    return *p;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    return (mem_[address] << 24) | (mem_[address + 1] << 16) | (mem_[address + 2] << 8) | mem_[address + 3];
                }
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
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u16*>(&mem_[address]);
                    *p = halfWord;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    mem_[address] = (halfWord >> 8) & 0xffu;
                    mem_[address + 1] = halfWord & 0xffu;
                }
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
                if constexpr (std::endian::native == std::endian::little)
                {
                    auto* p = reinterpret_cast<u32*>(&mem_[address]);
                    *p = word;
                }
                else if constexpr (std::endian::native == std::endian::big)
                {
                    mem_[address] = (word >> 24) & 0xffu;
                    mem_[address + 1] = (word >> 16) & 0xffu;
                    mem_[address + 2] = (word >> 8) & 0xffu;
                    mem_[address + 3] = word & 0xffu;
                }
            }
            else
            {
                throw TrappedException(TrapType::StoreAccessFault);
            }
        }
    };

} // namespace arviss
