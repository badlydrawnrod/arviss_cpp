#pragma once

#include "arviss/core/concepts.h"
#include "arviss/rv32/concepts.h"

namespace arviss::remix
{
    /*

    RISC-V instructions are encoded in a way that works well for hardware, but is time consuming to decode in software.
    This means that a directly interpreted implementation of Arviss spends a sizable portion of its time decoding and
    dispatching RISC-V instructions.

    Remix is a re-encoding of RISC-V instructions to make them easier to decode and dispatch from software by making it
    possible to use the opcode as an index into a single jump table, rather than having to mask, dispatch, possibly
    fail, mask again, and so on.

    It is also memory efficient, because it takes advantage of knowing that RISC-V instructions are encoded with the
    bottom two bits always set, which makes it possible for Remix and RISC-V encodings to co-exist in memory rather than
    having to cache Remix-encoded instructions and increase memory usage.

    Remix instructions are encoded as follows:

    +---+---+---+--------+
    | c | b | a | opcode |
    +---+---+---+--------+

    where:
    - opcode is 7 bits, right aligned.
    - a is 5 bits, right aligned.
    - b is 5 bits, right aligned.
    - c is a variable number of bits, left aligned to simplify sign extension as c is often an immediate value.
    - c takes precedence over b and a, so if their bits overlap it's like the overlapped item isn't present.

    As all opcodes ending in 0b11 are placeholders for RV32 instructions, this means that there can only be 96 Remix
    opcodes rather than 128, but as the instruction set is small this is not really a problem.

    */

    enum Opcode : uint32_t
    {
        Illegal,
        Beq,
        Bne,
        Rv03 = 0b000'0011,
        Blt,
        Bge,
        Bltu,
        Rv07 = 0b000'0111,
        Bgeu,
        Lb,
        Lh,
        Rv0b = 0b000'1011,
        Lw,
        Lbu,
        Lhu,
        Rv0f = 0b000'1111,
        Addi,
        Slti,
        Sltiu,
        Rv13 = 0b001'0011,
        Xori,
        Ori,
        Andi,
        Rv17 = 0b001'0111,
        Jalr,
        Sb,
        Sh,
        Rv1b = 0b001'1011,
        Sw,
        Auipc,
        Lui,
        Rv1f = 0b001'1111,
        Jal,
        Add,
        Sub,
        Rv23 = 0b010'0011,
        Sll,
        Slt,
        Sltu,
        Rv27 = 0b010'0111,
        Xor,
        Srl,
        Sra,
        Rv2b = 0b010'1011,
        Or,
        And,
        Slli,
        Rv2f = 0b010'1111,
        Srli,
        Srai,
        Fence,
        Rv33 = 0b011'0011,
        Ecall,
        Ebreak,
        Mul,
        Rv37 = 0b011'0111,
        Mulh,
        Mulhsu,
        Mulhu,
        Rv3b = 0b011'1011,
        Div,
        Divu,
        Rem,
        Rv3f = 0b011'1111,
        Remu,
        Fmv_x_w,
        Fclass_s,
        Rv43 = 0b100'0011,
        Fmv_w_x,
        Fsqrt_s,
        Fcvt_w_s,
        Rv47 = 0b100'0111,
        Fcvt_wu_s,
        Fcvt_s_w,
        Fcvt_s_wu,
        Rv4b = 0b100'1011,
        Fsgnj_s,
        Fsgnjn_s,
        Fsgnjx_s,
        Rv4f = 0b100'1111,
        Fmin_s,
        Fmax_s,
        Fle_s,
        Rv53 = 0b101'0011,
        Flt_s,
        Feq_s,
        Fadd_s,
        Rv57 = 0b101'0111,
        Fsub_s,
        Fmul_s,
        Fdiv_s,
        Rv5b = 0b101'1011,
        Flw,
        Fsw,
        Fmadd_s,
        Rv5f = 0b101'1111,
        Fmsub_s,
        Fnmsub_s,
        Fnmadd_s,
    };

    struct F0
    {
        uint32_t _ : 25;  // unused
        uint32_t opc : 7; // opcode
    };
    static_assert(sizeof(F0) == sizeof(uint32_t));

    struct F1a
    {
        uint32_t rs2 : 5; // rs2
        uint32_t _ : 10;  // unused
        uint32_t rs1 : 5; // rs1
        uint32_t rd : 5;  // rd
        uint32_t opc : 7; // opcode
    };
    static_assert(sizeof(F1a) == sizeof(uint32_t));

    struct F1s
    {
        uint32_t shamtw : 5; // shamtw
        uint32_t _ : 10;     // unused
        uint32_t rs1 : 5;    // rs1
        uint32_t rd : 5;     // rd
        uint32_t opc : 7;    // opcode
    };
    static_assert(sizeof(F1s) == sizeof(uint32_t));

    struct F2b
    {
        uint32_t bimm : 12; // bimmediate
        uint32_t _ : 3;     // unused
        uint32_t rs2 : 5;   // rs2
        uint32_t rs1 : 5;   // rs1
        uint32_t opc : 7;   // opcode
    };
    static_assert(sizeof(F2b) == sizeof(uint32_t));

    struct F2s
    {
        uint32_t simm : 12; // simmediate
        uint32_t _ : 3;     // unused
        uint32_t rs2 : 5;   // rs2
        uint32_t rs1 : 5;   // rs1
        uint32_t opc : 7;   // opcode
    };
    static_assert(sizeof(F2s) == sizeof(uint32_t));

    struct F2i
    {
        uint32_t iimm : 12; // iimmediate
        uint32_t _ : 3;     // unused
        uint32_t rs1 : 5;   // rs1
        uint32_t rd : 5;    // rd
        uint32_t opc : 7;   // opcode
    };
    static_assert(sizeof(F2i) == sizeof(uint32_t));

    struct F3
    {
        uint32_t fm : 4;  // fm
        uint32_t _ : 11;  // unused
        uint32_t rs1 : 5; // rs1
        uint32_t rd : 5;  // rd
        uint32_t opc : 7; // opcode
    };
    static_assert(sizeof(F3) == sizeof(uint32_t));

    struct F4j
    {
        uint32_t jimm : 20; // jimmediate
        uint32_t rd : 5;    // rd
        uint32_t opc : 7;   // opcode
    };
    static_assert(sizeof(F4j) == sizeof(uint32_t));

    struct F4u
    {
        uint32_t uimm : 20; // uimmediate
        uint32_t rd : 5;    // rd
        uint32_t opc : 7;   // opcode
    };
    static_assert(sizeof(F4u) == sizeof(uint32_t));

    struct Remix
    {
        union
        {
            F0 f0;
            F1a f1a;
            F1s f1s;
            F2b f2b;
            F2s f2s;
            F2i f2i;
            F3 f3;
            F4j f4j;
            F4u f4u;
        };
    };
    static_assert(sizeof(Remix) == sizeof(uint32_t));

    // An Rv32i instruction handler that re-encodes instructions for Remix.
    class Rv32iToRemixConverter
    {
    public:
        using Item = Remix;

        // Illegal instruction.

        auto Illegal(u32 ins) -> Item { return {.f0 = {._ = 0, .opc = Opcode::Illegal}}; }

        // B-type instructions.

        auto Beq(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.f2b = {.bimm = bimm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Beq}}; }
        auto Bne(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.f2b = {.bimm = bimm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Bne}}; }
        auto Blt(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.f2b = {.bimm = bimm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Blt}}; }
        auto Bge(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.f2b = {.bimm = bimm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Bge}}; }
        auto Bltu(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.f2b = {.bimm = bimm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Bltu}}; }
        auto Bgeu(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.f2b = {.bimm = bimm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Bgeu}}; }

        // I-type instructions.

        auto Lb(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Lb}}; }
        auto Lh(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Lh}}; }
        auto Lw(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Lw}}; }
        auto Lbu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Lbu}}; }
        auto Lhu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Lhu}}; }
        auto Addi(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Addi}}; }
        auto Slti(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Slti}}; }
        auto Sltiu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Sltiu}}; }
        auto Xori(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Xori}}; }
        auto Ori(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Ori}}; }
        auto Andi(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Andi}}; }
        auto Jalr(Reg rd, Reg rs1, u32 iimm) -> Item { return {.f2i = {.iimm = iimm, ._ = 0, .rs1 = rs1, .rd = rs1, .opc = Opcode::Jalr}}; }

        // S-type instructions.

        auto Sb(Reg rs1, Reg rs2, u32 simm) -> Item { return {.f2s = {.simm = simm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Sb}}; }
        auto Sh(Reg rs1, Reg rs2, u32 simm) -> Item { return {.f2s = {.simm = simm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Sh}}; }
        auto Sw(Reg rs1, Reg rs2, u32 simm) -> Item { return {.f2s = {.simm = simm, ._ = 0, .rs2 = rs2, .rs1 = rs1, .opc = Opcode::Sw}}; }

        // U-type instructions.

        auto Auipc(Reg rd, u32 uimm) -> Item { return {.f4u = {.uimm = uimm, .rd = rd, .opc = Opcode::Auipc}}; }
        auto Lui(Reg rd, u32 uimm) -> Item { return {.f4u = {.uimm = uimm, .rd = rd, .opc = Opcode::Lui}}; }

        // J-type instructions.

        auto Jal(Reg rd, u32 jimm) -> Item { return {.f4j = {.jimm = jimm, .rd = rd, .opc = Opcode::Jal}}; }

        // Arithmetic instructions.

        auto Add(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Add}}; }
        auto Sub(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Sub}}; }
        auto Sll(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Sll}}; }
        auto Slt(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Slt}}; }
        auto Sltu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Sltu}}; }
        auto Xor(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Xor}}; }
        auto Srl(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Srl}}; }
        auto Sra(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Sra}}; }
        auto Or(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Or}}; }
        auto And(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f1a = {.rs2 = rs2, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::And}}; }

        // Immediate shift instructions.

        auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item { return {.f1s = {.shamtw = shamt, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Slli}}; }
        auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item { return {.f1s = {.shamtw = shamt, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Srli}}; }
        auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item { return {.f1s = {.shamtw = shamt, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Srai}}; }

        // Fence instructions.

        auto Fence(u32 fm, Reg rd, Reg rs1) -> Item { return {.f3 = {.fm = fm, ._ = 0, .rs1 = rs1, .rd = rd, .opc = Opcode::Fence}}; }

        // System instructions.

        auto Ecall() -> Item { return {.f0 = {._ = 0, .opc = Opcode::Ecall}}; }
        auto Ebreak() -> Item { return {.f0 = {._ = 0, .opc = Opcode::Ebreak}}; }
    };

    static_assert(IsRv32iInstructionHandler<Rv32iToRemixConverter>);

    // // An Rv32im instruction handler that re-encodes instructions for Arviss.
    // class Rv32imToRemixConverter : public Rv32iToRemixConverter
    // {
    // public:
    //     using Item = Rv32iToRemixConverter::Item;

    //     auto Mul(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Mul, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Mulh(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Mulh, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Mulhsu(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Mulhsu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Mulhu(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Mulhu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Div(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Div, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Divu(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Divu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Rem(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Rem, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Remu(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Remu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    // };

    // static_assert(IsRv32imInstructionHandler<Rv32imToRemixConverter>);

    // class Rv32imfToRemixConverter : public Rv32imToRemixConverter
    // {
    // public:
    //     using Item = Rv32imToRemixConverter::Item;

    //     auto Fmv_x_w(Reg rd, Reg rs1) -> Item { return {.opcode = Opcode::Fmv_x_w, .floatRdRs1{.rd = rd, .rs1 =
    //     rs1}}; } auto Fclass_s(Reg rd, Reg rs1) -> Item
    //     {
    //         return {.opcode = Opcode::Fclass_s, .floatRdRs1{.rd = rd, .rs1 = rs1}};
    //     }
    //     auto Fmv_w_x(Reg rd, Reg rs1) -> Item { return {.opcode = Opcode::Fmv_w_x, .floatRdRs1{.rd = rd, .rs1 =
    //     rs1}}; } auto Fsqrt_s(Reg rd, Reg rs1, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fsqrt_s, .floatRdRs1Rm{.rd = rd, .rs1 = rs1, .rm = rm}};
    //     }
    //     auto Fcvt_w_s(Reg rd, Reg rs1, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fcvt_w_s, .floatRdRs1Rm{.rd = rd, .rs1 = rs1, .rm = rm}};
    //     }
    //     auto Fcvt_wu_s(Reg rd, Reg rs1, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fcvt_wu_s, .floatRdRs1Rm{.rd = rd, .rs1 = rs1, .rm = rm}};
    //     }
    //     auto Fcvt_s_w(Reg rd, Reg rs1, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fcvt_s_w, .floatRdRs1Rm{.rd = rd, .rs1 = rs1, .rm = rm}};
    //     }
    //     auto Fcvt_s_wu(Reg rd, Reg rs1, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fcvt_s_wu, .floatRdRs1Rm{.rd = rd, .rs1 = rs1, .rm = rm}};
    //     }
    //     auto Fsgnj_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Fsgnj_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Fsgnjn_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Fsgnjn_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Fsgnjx_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Fsgnjx_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Fmin_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Fmin_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Fmax_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Fmax_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Fle_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Fle_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Flt_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Flt_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }
    //     auto Feq_s(Reg rd, Reg rs1, Reg rs2) -> Item
    //     {
    //         return {.opcode = Opcode::Feq_s, .floatRdRs1Rs2{.rd = rd, .rs1 = rs1, .rs2 = rs2}};
    //     }

    //     auto Fadd_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fadd_s, .floatRdRs1Rs2Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rm = rm}};
    //     }

    //     auto Fsub_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fsub_s, .floatRdRs1Rs2Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rm = rm}};
    //     }

    //     auto Fmul_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fmul_s, .floatRdRs1Rs2Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rm = rm}};
    //     }

    //     auto Fdiv_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fdiv_s, .floatRdRs1Rs2Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rm = rm}};
    //     }

    //     auto Flw(Reg rd, Reg rs1, u32 imm) -> Item
    //     {
    //         return {.opcode = Opcode::Flw, .floatRdRs1Imm{.rd = rd, .rs1 = rs1, .imm = imm}};
    //     }
    //     auto Fsw(Reg rs1, Reg rs2, u32 imm) -> Item
    //     {
    //         return {.opcode = Opcode::Fsw, .floatRs1Rs2Imm{.rs1 = rs1, .rs2 = rs2, .imm = imm}};
    //     }

    //     auto Fmadd_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fmadd_s,
    //                 .floatRdRs1Rs2Rs3Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rs3 = rs3, .rm = rm}};
    //     }

    //     auto Fmsub_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fmsub_s,
    //                 .floatRdRs1Rs2Rs3Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rs3 = rs3, .rm = rm}};
    //     }

    //     auto Fnmsub_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fnmsub_s,
    //                 .floatRdRs1Rs2Rs3Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rs3 = rs3, .rm = rm}};
    //     }

    //     auto Fnmadd_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item
    //     {
    //         return {.opcode = Opcode::Fnmadd_s,
    //                 .floatRdRs1Rs2Rs3Rm{.rd = rd, .rs1 = rs1, .rs2 = rs2, .rs3 = rs3, .rm = rm}};
    //     }
    // };

    // static_assert(IsRv32imfInstructionHandler<Rv32imfToRemixConverter>);

} // namespace arviss::remix
