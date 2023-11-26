#pragma once

#include "types.h"

// TYPES: not at all specific.

inline auto Sext(u32 n, i32 topBit) -> u32
{
    auto shift = 31 - topBit;
    return static_cast<u32>((static_cast<i32>(n << shift) >> shift));
}

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

    // RV32C

    auto Rdp() -> u32 const { return (ins_ >> 2) & 7; }

    auto Rdn0() -> u32 const { return Rd(); }

    auto Rdn2() -> u32 const { return Rd(); }

    auto Rdrs1() -> u32 const { return Rd(); }

    auto Rs1p() -> u32 const { return (ins_ >> 7) & 7; }

    auto Rs2p() -> u32 const { return (ins_ >> 2) & 7; }

    auto Rdrs1p() -> u32 const { return (ins_ >> 7) & 7; }

    auto Rs1n0() -> u32 const { return ins_ >> 7; }

    auto Rs2n0() -> u32 const { return ins_ >> 2; }

    auto Rdrs1n0() -> u32 const { return Rd(); }

    auto C_rs2() -> u32 const { return ins_ >> 2; }

    auto C_nzuimm10() -> u32
    {
        // Zero extended.
        auto imm = (ins_ >> 5) & 0xff;
        auto a = ((imm & 0b11000000) >> 6) << 4; // 5:4
        auto b = ((imm & 0b00111100) >> 2) << 6; // 9:6
        auto c = ((imm & 0b00000010) >> 1) << 2; // 2
        auto d = (imm & 0b00000001) << 3;        // 3
        return a | b | c | d;
    }

    auto C_uimm7() -> u32
    {
        // Zero extended.
        auto a = ((ins_ >> 12) & 1) << 5;      // offset[5]
        auto b = ((ins_ & 0b11100) >> 2) << 3; // offset[4:3]
        auto c = (ins_ & 0b00011) << 6;        // offset[7:6]
        return a | b | c;
    }

    auto C_nzimm6() -> u32
    {
        // Sign extended.
        auto a = ((ins_ >> 12) & 1) << 5; // imm[5]
        auto b = (ins_ >> 2) & 0x1f;      // imm[4:0]
        return Sext(a | b, 5);
    }

    auto C_nzimm10() -> u32
    {
        // Sign extended.
        auto a = ((ins_ >> 12) & 1) << 9; // nzimm[9]
        auto imm = ((ins_) >> 2) & 0x1f;
        auto b = ((imm & 0b10000) >> 4) << 4; // nzimm[4]
        auto c = ((imm & 0b01000) >> 3) << 6; // nzimm[6]
        auto d = ((imm & 0b00110) >> 1) << 7; // nzimm[8:7]
        auto e = (imm & 0b00001) << 5;        // nzimm[5]
        return Sext(a | b | c | d | e, 9);
    }

    auto C_nzimm18() -> u32
    {
        // Sign extended.
        auto a = ((ins_ >> 12) & 1) << 17;   // nzimm[17]
        auto b = ((ins_ >> 2) & 0x1f) << 12; // nzimm[16:12]
        return Sext(a | b, 17);
    }

    auto C_imm6() -> u32
    {
        // Sign extended.
        auto a = ((ins_ >> 12) & 1) << 5; // imm[5]
        auto b = (ins_ >> 2) & 0x1f;      // imm[4:0]
        return Sext(a | b, 5);
    }

    auto C_imm12() -> u32
    {
        // Sign extended.
        auto imm = (ins_ >> 2) & 0x7ff;
        auto a = ((imm & 0b10000000000) >> 10) << 11; // offset[11]
        auto b = ((imm & 0b01000000000) >> 9) << 4;   // offset[4]
        auto c = ((imm & 0b00110000000) >> 7) << 8;   // offset[9:8]
        auto d = ((imm & 0b00001000000) >> 6) << 10;  // offset[10]
        auto e = ((imm & 0b00000100000) >> 5) << 6;   // offset[6]
        auto f = ((imm & 0b00000010000) >> 4) << 7;   // offset[7]
        auto g = ((imm & 0b00000001110) >> 1) << 1;   // offset[3:1]
        auto h = (imm & 0b00000000001) << 5;          // offset[5]
        return Sext(a | b | c | d | e | f | g | h, 11);
    }

    auto C_bimm9() -> u32
    {
        // Sign extended.
        auto imm1 = (ins_ >> 10) & 7;
        auto a = ((imm1 & 0b100) >> 2) << 8; // offset[8]
        auto b = (imm1 & 0b011) << 3;        // offset[4:3]
        auto imm2 = (ins_ >> 2) & 0x1f;
        auto c = ((imm2 & 0b11000) >> 3) << 6; // offset[7:6]
        auto d = ((imm2 & 0b00110) >> 1) << 1; // offset[2:1]
        auto e = (imm2 & 0b00001) << 5;        // offset[5]
        return Sext(a | b | c | d | e, 8);
    }

    auto C_uimm8sp() -> u32
    {
        // Zero extended.
        auto a = ((ins_ >> 12) & 1) << 5; // offset[5]
        auto imm = (ins_ >> 2) & 0x1f;
        auto b = ((imm & 0b11100) >> 2) << 2; // offset[4:2]
        auto c = (imm & 0b00011) << 6;        // offset[7:6]
        return a | b | c;
    }

    auto C_uimm8sp_s() -> u32
    {
        // Zero extended.
        auto imm = (ins_ >> 7) & 0x3f;
        auto a = ((imm & 0b111100) >> 2) << 2; // offset[5:2]
        auto b = (imm & 0b000011) << 6;        // offset[7:6]
        return a | b;
    }

    auto C_nzuimm6() -> u32
    {
        // Zero extended.
        auto a = ((ins_ >> 12) & 1) << 5; // shamt[5]
        auto b = (ins_ >> 2) & 0x1f;      // shamt[4:0]
        return a | b;
    }
};
