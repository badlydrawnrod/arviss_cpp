#pragma once

#include "types.h"

// TYPES: not at all specific.

// A helper for breaking down instructions.
class Instruction
{
    u32 ins_{};

    auto Bits(u32 hi, u32 lo) -> u32
    {
        auto n = ins_;
        auto run = (hi - lo) + 1;
        auto mask = ((1 << run) - 1) << lo;
        return (n & mask) >> lo;
    }

public:
    Instruction(u32 ins) : ins_{ins} {};

    auto Opcode() -> u32 const { return ins_ & 0x7f; }

    auto Rd() -> u32 const { return (ins_ >> 7) & 0x1f; }
    auto Rs1() -> u32 const { return (ins_ >> 15) & 0x1f; }
    auto Rs2() -> u32 const { return (ins_ >> 20) & 0x1f; }

    auto Shamtw() -> u32 const { return (ins_ >> 20) & 0x1f; }

    auto Fm() -> u32 const { return (ins_ >> 20) & 0xf; }

    auto Bimmediate() -> u32 const
    {
        auto imm12 = static_cast<i32>(ins_ & 0x80000000) >> 19;     // ins[31] -> sext(imm[12])
        auto imm11 = static_cast<i32>((ins_ & 0x00000080) << 4);    // ins[7] -> imm[11]
        auto imm10_5 = static_cast<i32>((ins_ & 0x7e000000) >> 20); // ins[30:25] -> imm[10:5]
        auto imm4_1 = static_cast<i32>((ins_ & 0x00000f00) >> 7);   // ins[11:8]  -> imm[4:1]
        return static_cast<u32>(imm12 | imm11 | imm10_5 | imm4_1);
    }

    auto Iimmediate() -> u32 const
    {
        return static_cast<u32>((static_cast<i32>(ins_) >> 20)); // ins[31:20] -> sext(imm[11:0])
    }

    auto Simmediate() -> u32 const
    {
        auto imm11_5 = (static_cast<i32>(ins_ & 0xfe000000)) >> 20; // ins[31:25] -> sext(imm[11:5])
        auto imm4_0 = static_cast<i32>((ins_ & 0x00000f80) >> 7);   // ins[11:7]  -> imm[4:0]
        return static_cast<u32>(imm11_5 | imm4_0);
    }

    auto Jimmediate() -> u32 const
    {
        auto imm20 = static_cast<i32>(ins_ & 0x80000000) >> 11;     // ins[31] -> sext(imm[20])
        auto imm19_12 = static_cast<i32>(ins_ & 0x000ff000);        // ins[19:12] -> imm[19:12]
        auto imm11 = static_cast<i32>((ins_ & 0x00100000) >> 9);    // ins[20] -> imm[11]
        auto imm10_1 = static_cast<i32>((ins_ & 0x7fe00000) >> 20); // ins[30:21] -> imm[10:1]
        return static_cast<u32>(imm20 | imm19_12 | imm11 | imm10_1);
    }

    auto Uimmediate() -> u32 const
    {
        return ins_ & 0xfffff000; // ins[31:12] -> imm[31:12]
    }
};
