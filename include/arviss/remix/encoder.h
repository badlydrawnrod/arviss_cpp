#pragma once

#include "arviss/arviss.h"
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

    Update: not all 'c' values are left aligned, e.g., floating point instructions.

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
        // +--------+-----+
        // | unused | opc |
        // |     25 |   7 |
        // +--------+-----+
        F0(Opcode opc) : v{opc} {}
        auto opc() const -> Opcode { return Opcode(v & 0x7f); }

    private:
        u32 v;
    };
    static_assert(sizeof(F0) == sizeof(uint32_t));

    struct F1a
    {
        // +-----+--------+-----+----+-----+
        // | rs2 | unused | rs1 | rd | opc |
        // |   5 |     10 |   5 |  5 |   7 |
        // +-----+--------+-----+----+-----+
        F1a(Opcode opc, u32 rd, u32 rs1, u32 rs2) : v{(rs2 << 27) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto rs2() const -> u32 { return (v >> 27) & 0x1f; }

    private:
        u32 v;
    };
    static_assert(sizeof(F1a) == sizeof(uint32_t));

    struct F1s
    {
        // +-------+--------+-----+----+-----+
        // | shamt | unused | rs1 | rd | opc |
        // |     5 |     10 |   5 |  5 |   7 |
        // +-------+--------+-----+----+-----+
        F1s(Opcode opc, u32 rd, u32 rs1, u32 shamt) : v{(shamt << 27) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto shamt() const -> u32 { return (v >> 27) & 0x1f; }

    private:
        u32 v;
    };
    static_assert(sizeof(F1s) == sizeof(uint32_t));

    struct F2b
    {
        // +------+--------+-----+-----+-----+
        // | bimm | unused | rs2 | rs1 | opc |
        // |   12 |      3 |   5 |   5 |   7 |
        // +------+--------+-----+-----+-----+

        // Note: shift bimm by 19 not 20 because the normal decoder has already done the multiply by 2
        F2b(Opcode opc, u32 rs1, u32 rs2, u32 bimm) : v{(bimm << 19) | (rs2 << 12) | (rs1 << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rs1() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs2() const -> u32 { return (v >> 12) & 0x1f; }
        auto bimm() const -> u32 { return u32(i32(v) >> 19) & 0xfffffffe; }

    private:
        u32 v;
    };
    static_assert(sizeof(F2b) == sizeof(uint32_t));

    struct F2s
    {
        // +------+--------+-----+-----+-----+
        // | simm | unused | rs2 | rs1 | opc |
        // |   12 |      3 |   5 |   5 |   7 |
        // +------+--------+-----+-----+-----+
        F2s(Opcode opc, u32 rs1, u32 rs2, u32 simm) : v{(simm << 20) | (rs2 << 12) | (rs1 << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rs1() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs2() const -> u32 { return (v >> 12) & 0x1f; }
        auto simm() const -> u32 { return u32(i32(v) >> 20); }

    private:
        u32 v;
    };
    static_assert(sizeof(F2s) == sizeof(uint32_t));

    struct F2i
    {
        // +------+--------+-----+----+-----+
        // | iimm | unused | rs1 | rd | opc |
        // |   12 |      3 |   5 |  5 |   7 |
        // +------+--------+-----+----+-----+
        F2i(Opcode opc, u32 rd, u32 rs1, u32 iimm) : v{(iimm << 20) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto iimm() const -> u32 { return u32(i32(v) >> 20); }

    private:
        u32 v;
    };
    static_assert(sizeof(F2i) == sizeof(uint32_t));

    struct F3
    {
        // +----+--------+-----+----+-----+
        // | fm | unused | rs1 | rd | opc |
        // |  4 |     11 |   5 |  5 |   7 |
        // +----+--------+-----+----+-----+
        F3(Opcode opc, u32 rd, u32 rs1, u32 fm) : v{(fm << 28) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto fm() const -> u32 { return (v >> 28) & 0xf; }

    private:
        u32 v;
    };
    static_assert(sizeof(F3) == sizeof(uint32_t));

    struct F4j
    {
        // +--------+----+-----+
        // | jimm20 | rd | opc |
        // |   20   |  5 |   7 |
        // +--------+----+-----+

        // Note: shift jimm by 11 not 12 because the normal decoder has already done the multiply by 2
        F4j(Opcode opc, u32 rd, u32 uimm) : v{(uimm << 11) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto jimm() const -> u32 { return u32(i32(v) >> 11) & 0xfffffffe; }

    private:
        u32 v;
    };
    static_assert(sizeof(F4j) == sizeof(uint32_t));

    struct F4u
    {
        // +--------+----+-----+
        // | uimm20 | rd | opc |
        // |   20   |  5 |   7 |
        // +--------+----+-----+

        // Note: don't shift uimm because the normal decoder has already done it.
        F4u(Opcode opc, u32 rd, u32 uimm) : v{uimm | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto uimm() const -> u32 { return v & 0xfffff000; }

    private:
        u32 v;
    };
    static_assert(sizeof(F4u) == sizeof(uint32_t));

    struct F5
    {
        // +--------+-----+----+-----+
        // | unused | rs1 | rd | opc |
        // |     15 |   5 |  5 |   7 |
        // +--------+-----+----+-----+
        F5(Opcode opc, u32 rd, u32 rs1) : v{(rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }

    private:
        u32 v;
    };
    static_assert(sizeof(F5) == sizeof(uint32_t));

    struct F5rm
    {
        // +--------+----+-----+----+-----+
        // | unused | rm | rs1 | rd | opc |
        // |     12 |  3 |   5 |  5 |   7 |
        // +--------+----+-----+----+-----+
        F5rm(Opcode opc, u32 rd, u32 rs1, u32 rm) : v{(rm << 17) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto rm() const -> u32 { return (v >> 17) & 0x7; }

    private:
        u32 v;
    };
    static_assert(sizeof(F5rm) == sizeof(uint32_t));

    struct F6
    {
        // +--------+-----+-----+----+-----+
        // | unused | rs2 | rs1 | rd | opc |
        // |     10 |   5 |   5 |  5 |   7 |
        // +--------+-----+-----+----+-----+
        F6(Opcode opc, u32 rd, u32 rs1, u32 rs2) : v{(rs2 << 17) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto rs2() const -> u32 { return (v >> 17) & 0x1f; }

    private:
        u32 v;
    };
    static_assert(sizeof(F6) == sizeof(uint32_t));

    struct F6rm
    {
        // +--------+----+-----+-----+----+-----+
        // | unused | rm | rs2 | rs1 | rd | opc |
        // |      7 |  3 |   5 |   5 |  5 |   7 |
        // +--------+----+-----+-----+----+-----+
        F6rm(Opcode opc, u32 rd, u32 rs1, u32 rs2, u32 rm) : v{(rm << 22) | (rs2 << 17) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto rs2() const -> u32 { return (v >> 17) & 0x1f; }
        auto rm() const -> u32 { return (v >> 22) & 0x7; }

    private:
        u32 v;
    };
    static_assert(sizeof(F6rm) == sizeof(uint32_t));

    struct F7
    {
        // +--------+----+-----+-----+-----+----+-----+
        // | unused | rm | rs3 | rs2 | rs1 | rd | opc |
        // |      2 |  3 |   5 |   5 |   5 |  5 |   7 |
        // +--------+----+-----+-----+-----+----+-----+
        F7(Opcode opc, u32 rd, u32 rs1, u32 rs2, u32 rs3, u32 rm) : v{(rm << 27) | (rs3 << 22) | (rs2 << 17) | (rs1 << 12) | (rd << 7) | opc} {}

        auto opc() const -> Opcode { return Opcode(v & 0x7f); }
        auto rd() const -> u32 { return (v >> 7) & 0x1f; }
        auto rs1() const -> u32 { return (v >> 12) & 0x1f; }
        auto rs2() const -> u32 { return (v >> 17) & 0x1f; }
        auto rs3() const -> u32 { return (v >> 22) & 0x1f; }
        auto rm() const -> u32 { return (v >> 27) & 0x7; }

    private:
        u32 v;
    };
    static_assert(sizeof(F7) == sizeof(uint32_t));

    struct Remix
    {
        union
        {
            F0 f0;
            F1a arithType;
            F1s immShiftType;
            F2b btype;
            F2s stype;
            F2i itype;
            F3 fenceType;
            F4j jtype;
            F4u utype;
            F5 f5Type;
            F5rm f5rmType;
            F6 f6Type;
            F6rm f6rmType;
            F7 f7Type;
        };
    };
    static_assert(sizeof(Remix) == sizeof(uint32_t));

    // An Rv32i instruction handler that re-encodes instructions for Remix.
    class Rv32iToRemixConverter
    {
    public:
        using Item = Remix;

        // Illegal instruction.

        auto Illegal(u32 /*ins*/) -> Item { return {.f0 = F0(Opcode::Illegal)}; }

        // B-type instructions.

        auto Beq(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.btype = F2b(Opcode::Beq, rs1, rs2, bimm)}; }
        auto Bne(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.btype = F2b(Opcode::Bne, rs1, rs2, bimm)}; }
        auto Blt(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.btype = F2b(Opcode::Blt, rs1, rs2, bimm)}; }
        auto Bge(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.btype = F2b(Opcode::Bge, rs1, rs2, bimm)}; }
        auto Bltu(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.btype = F2b(Opcode::Bltu, rs1, rs2, bimm)}; }
        auto Bgeu(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.btype = F2b(Opcode::Bgeu, rs1, rs2, bimm)}; }

        // I-type instructions.

        auto Lb(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Lb, rd, rs1, iimm)}; }
        auto Lh(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Lh, rd, rs1, iimm)}; }
        auto Lw(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Lw, rd, rs1, iimm)}; }
        auto Lbu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Lbu, rd, rs1, iimm)}; }
        auto Lhu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Lhu, rd, rs1, iimm)}; }
        auto Addi(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Addi, rd, rs1, iimm)}; }
        auto Slti(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Slti, rd, rs1, iimm)}; }
        auto Sltiu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Sltiu, rd, rs1, iimm)}; }
        auto Xori(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Xori, rd, rs1, iimm)}; }
        auto Ori(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Ori, rd, rs1, iimm)}; }
        auto Andi(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Andi, rd, rs1, iimm)}; }
        auto Jalr(Reg rd, Reg rs1, u32 iimm) -> Item { return {.itype = F2i(Opcode::Jalr, rd, rs1, iimm)}; }

        // S-type instructions.

        auto Sb(Reg rs1, Reg rs2, u32 simm) -> Item { return {.stype = F2s(Opcode::Sb, rs1, rs2, simm)}; }
        auto Sh(Reg rs1, Reg rs2, u32 simm) -> Item { return {.stype = F2s(Opcode::Sh, rs1, rs2, simm)}; }
        auto Sw(Reg rs1, Reg rs2, u32 simm) -> Item { return {.stype = F2s(Opcode::Sw, rs1, rs2, simm)}; }

        // U-type instructions.

        auto Auipc(Reg rd, u32 uimm) -> Item { return {.utype = F4u(Opcode::Auipc, rd, uimm)}; }
        auto Lui(Reg rd, u32 uimm) -> Item { return {.utype = F4u(Opcode::Lui, rd, uimm)}; }

        // J-type instructions.

        auto Jal(Reg rd, u32 jimm) -> Item { return {.jtype = F4j(Opcode::Jal, rd, jimm)}; }

        // Arithmetic instructions.

        auto Add(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Add, rd, rs1, rs2)}; }
        auto Sub(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Sub, rd, rs1, rs2)}; }
        auto Sll(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Sll, rd, rs1, rs2)}; }
        auto Slt(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Slt, rd, rs1, rs2)}; }
        auto Sltu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Sltu, rd, rs1, rs2)}; }
        auto Xor(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Xor, rd, rs1, rs2)}; }
        auto Srl(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Srl, rd, rs1, rs2)}; }
        auto Sra(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Sra, rd, rs1, rs2)}; }
        auto Or(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Or, rd, rs1, rs2)}; }
        auto And(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::And, rd, rs1, rs2)}; }

        // Immediate shift instructions.

        auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item { return {.immShiftType = F1s(Opcode::Slli, rd, rs1, shamt)}; }
        auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item { return {.immShiftType = F1s(Opcode::Srli, rd, rs1, shamt)}; }
        auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item { return {.immShiftType = F1s(Opcode::Srai, rd, rs1, shamt)}; }

        // Fence instructions.

        auto Fence(u32 fm, Reg rd, Reg rs1) -> Item { return {.fenceType = F3(Opcode::Fence, rd, rs1, fm)}; }

        // System instructions.

        auto Ecall() -> Item { return {.f0 = F0(Opcode::Ecall)}; }
        auto Ebreak() -> Item { return {.f0 = F0(Opcode::Ebreak)}; }
    };

    static_assert(IsRv32iHandler<Rv32iToRemixConverter>);

    // An Rv32im instruction handler that re-encodes instructions for Remix.
    class Rv32imToRemixConverter : public Rv32iToRemixConverter
    {
    public:
        using Item = Rv32iToRemixConverter::Item;

        auto Mul(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Mul, rd, rs1, rs2)}; }
        auto Mulh(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Mulh, rd, rs1, rs2)}; }
        auto Mulhsu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Mulhsu, rd, rs1, rs2)}; }
        auto Mulhu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Mulhu, rd, rs1, rs2)}; }
        auto Div(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Div, rd, rs1, rs2)}; }
        auto Divu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Divu, rd, rs1, rs2)}; }
        auto Rem(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Rem, rd, rs1, rs2)}; }
        auto Remu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.arithType = F1a(Opcode::Remu, rd, rs1, rs2)}; }
    };

    static_assert(IsRv32imHandler<Rv32imToRemixConverter>);

    // An Rv32imf instruction handler that re-encodes instructions for Remix.
    class Rv32imfToRemixConverter : public Rv32imToRemixConverter
    {
    public:
        using Item = Rv32imToRemixConverter::Item;

        auto Fmv_x_w(Reg rd, Reg rs1) -> Item { return {.f5Type = F5(Opcode::Fmv_x_w, rd, rs1)}; }
        auto Fclass_s(Reg rd, Reg rs1) -> Item { return {.f5Type = F5(Opcode::Fclass_s, rd, rs1)}; }
        auto Fmv_w_x(Reg rd, Reg rs1) -> Item { return {.f5Type = F5(Opcode::Fmv_w_x, rd, rs1)}; }

        auto Fsqrt_s(Reg rd, Reg rs1, u32 rm) -> Item { return {.f5rmType = F5rm(Opcode::Fsqrt_s, rd, rs1, rm)}; }
        auto Fcvt_w_s(Reg rd, Reg rs1, u32 rm) -> Item { return {.f5rmType = F5rm(Opcode::Fcvt_w_s, rd, rs1, rm)}; }
        auto Fcvt_wu_s(Reg rd, Reg rs1, u32 rm) -> Item { return {.f5rmType = F5rm(Opcode::Fcvt_wu_s, rd, rs1, rm)}; }
        auto Fcvt_s_w(Reg rd, Reg rs1, u32 rm) -> Item { return {.f5rmType = F5rm(Opcode::Fcvt_s_w, rd, rs1, rm)}; }
        auto Fcvt_s_wu(Reg rd, Reg rs1, u32 rm) -> Item { return {.f5rmType = F5rm(Opcode::Fcvt_s_wu, rd, rs1, rm)}; }

        auto Fsgnj_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Fsgnj_s, rd, rs1, rs2)}; }
        auto Fsgnjn_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Fsgnjn_s, rd, rs1, rs2)}; }
        auto Fsgnjx_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Fsgnjx_s, rd, rs1, rs2)}; }
        auto Fmin_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Fmin_s, rd, rs1, rs2)}; }
        auto Fmax_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Fmax_s, rd, rs1, rs2)}; }
        auto Fle_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Fle_s, rd, rs1, rs2)}; }
        auto Flt_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Flt_s, rd, rs1, rs2)}; }
        auto Feq_s(Reg rd, Reg rs1, Reg rs2) -> Item { return {.f6Type = F6(Opcode::Feq_s, rd, rs1, rs2)}; }

        auto Fadd_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item { return {.f6rmType = F6rm(Opcode::Fadd_s, rd, rs1, rs2, rm)}; }
        auto Fsub_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item { return {.f6rmType = F6rm(Opcode::Fsub_s, rd, rs1, rs2, rm)}; }
        auto Fmul_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item { return {.f6rmType = F6rm(Opcode::Fmul_s, rd, rs1, rs2, rm)}; }
        auto Fdiv_s(Reg rd, Reg rs1, Reg rs2, u32 rm) -> Item { return {.f6rmType = F6rm(Opcode::Fdiv_s, rd, rs1, rs2, rm)}; }

        auto Flw(Reg rd, Reg rs1, u32 imm) -> Item { return {.itype = F2i(Opcode::Flw, rd, rs1, imm)}; }

        auto Fsw(Reg rs1, Reg rs2, u32 imm) -> Item { return {.stype = F2s(Opcode::Fsw, rs1, rs2, imm)}; }

        auto Fmadd_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item { return {.f7Type = F7(Opcode::Fmadd_s, rd, rs1, rs2, rs3, rm)}; }
        auto Fmsub_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item { return {.f7Type = F7(Opcode::Fmsub_s, rd, rs1, rs2, rs3, rm)}; }
        auto Fnmsub_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item { return {.f7Type = F7(Opcode::Fnmsub_s, rd, rs1, rs2, rs3, rm)}; }
        auto Fnmadd_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 rm) -> Item { return {.f7Type = F7(Opcode::Fnmadd_s, rd, rs1, rs2, rs3, rm)}; }
    };

    static_assert(IsRv32imfHandler<Rv32imfToRemixConverter>);

} // namespace arviss::remix
