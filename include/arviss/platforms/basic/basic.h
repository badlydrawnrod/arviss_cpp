#pragma once

#include "arviss/arviss.h"

#include <bit>
#include <iostream>
#include <vector>

namespace arviss::platforms::basic
{
    // If you're not an a little-endian platform or a big-endian platform then all bets are off.
    static_assert(std::endian::native == std::endian::little || std::endian::native == std::endian::big);

    // ROM and RAM.
    constexpr Address ROM_START = 0;
    constexpr Address RAM_START = 0x4000;

    constexpr size_t MEM_SIZE = 0x8000;

    // Memory mapped I/O.
    constexpr Address TTY_STATUS = 0x8000;
    constexpr Address TTY_DATA = 0x8001;

    namespace impl
    {
        // A mixin implementation of a simple, checked address space that can signal bad access. It can have simple TTY
        // output, but that can be turned off for benchmarking purposes.
        template<bool has_io = false>
        class Memory
        {
            // 32KiB of memory. The first 16KiB is read-only.
            std::vector<u8> mem_ = std::vector<u8>(MEM_SIZE);

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

            auto Write8Unprotected(Address address, u8 byte) -> void
            {
                if (address < MEM_SIZE)
                {
                    mem_[address] = byte;
                }
                else if (address == TTY_DATA)
                {
                    if constexpr (has_io)
                    {
                        std::cout << static_cast<char>(byte);
                    }
                }
                else
                {
                    throw TrappedException(TrapType::StoreAccessFault);
                }
            }

            auto Write8(Address address, u8 byte) -> void
            {
                if (address >= RAM_START)
                {
                    // It's not in the ROM.
                    return Write8Unprotected(address, byte);
                }
                throw TrappedException(TrapType::StoreAccessFault);
            }

            auto Write16Unprotected(Address address, u16 halfWord) -> void
            {
                if (address < MEM_SIZE - 1)
                {
                    if constexpr (std::endian::native == std::endian::little)
                    {
                        auto* p = reinterpret_cast<u16*>(&mem_[address]);
                        *p = halfWord;
                    }
                    else if constexpr (std::endian::native == std::endian::big)
                    {
                        mem_[address] = (halfWord >> 8u) & 0xffu;
                        mem_[address + 1] = halfWord & 0xffu;
                    }
                }
                else
                {
                    throw TrappedException(TrapType::StoreAccessFault);
                }
            }

            auto Write16(Address address, u16 halfWord) -> void
            {
                if (address >= RAM_START)
                {
                    // It's not in the ROM.
                    return Write16Unprotected(address, halfWord);
                }
                throw TrappedException(TrapType::StoreAccessFault);
            }

            auto Write32Unprotected(Address address, u32 word) -> void
            {
                if (address < MEM_SIZE - 3)
                {
                    if constexpr (std::endian::native == std::endian::little)
                    {
                        auto* p = reinterpret_cast<u32*>(&mem_[address]);
                        *p = word;
                    }
                    else if constexpr (std::endian::native == std::endian::big)
                    {
                        mem_[address] = (word >> 24) & 0xff;
                        mem_[address + 1] = (word >> 16) & 0xff;
                        mem_[address + 2] = (word >> 8) & 0xff;
                        mem_[address + 3] = word & 0xff;
                    }
                }
                else
                {
                    throw TrappedException(TrapType::StoreAccessFault);
                }
            }

            auto Write32(Address address, u32 word) -> void
            {
                if (address >= RAM_START)
                {
                    // It's not in the ROM.
                    return Write32Unprotected(address, word);
                }
                throw TrappedException(TrapType::StoreAccessFault);
            }
        };
    } // namespace impl

    using Memory = impl::Memory<true>;
    using MemoryNoIO = impl::Memory<false>;

} // namespace arviss::platforms::basic
