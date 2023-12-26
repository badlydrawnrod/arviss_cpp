#pragma once

#include "arviss/core/concepts.h"
#include "arviss/rv32/concepts.h"

namespace arviss
{
    // Arviss-encode opcodes.

    enum class Opcode
    {
        // Arviss.
        Fdx,

        // --- RV32i

        // Illegal instruction.
        Illegal,
        // B-type instructions.
        Beq,
        Bne,
        Blt,
        Bge,
        Bltu,
        Bgeu,
        // I-type instructions.
        Lb,
        Lh,
        Lw,
        Lbu,
        Lhu,
        Addi,
        Slti,
        Sltiu,
        Xori,
        Ori,
        Andi,
        Jalr,
        // S-type instructions.
        Sb,
        Sh,
        Sw,
        // U-type instructions.
        Auipc,
        Lui,
        // J-type instructions.
        Jal,
        // Arithmetic instructions.
        Add,
        Sub,
        Sll,
        Slt,
        Sltu,
        Xor,
        Srl,
        Sra,
        Or,
        And,
        // Immediate shift instructions.
        Slli,
        Srli,
        Srai,
        Fence,
        Ecall,
        Ebreak,

        // --- RV32m
        Mul,
        Mulh,
        Mulhsu,
        Mulhu,
        Div,
        Divu,
        Rem,
        Remu
    };

    // Illegal instruction.
    struct IllegalType
    {
        u32 ins;
    };

    // B-type instruction.
    struct BType
    {
        Reg rs1;
        Reg rs2;
        u32 bimm;
    };

    // I-type instruction.
    struct IType
    {
        Reg rd;
        Reg rs1;
        u32 iimm;
    };

    // S-type instruction.
    struct SType
    {
        Reg rs1;
        Reg rs2;
        u32 simm;
    };

    // U-type instruction.
    struct UType
    {
        Reg rd;
        Reg uimm;
    };

    // J-type instruction.
    struct JType
    {
        Reg rd;
        u32 jimm;
    };

    // Arithmetic instruction.
    struct ArithType
    {
        Reg rd;
        Reg rs1;
        Reg rs2;
    };

    // Immediate shift instruction.
    struct ImmShiftType
    {
        Reg rd;
        Reg rs1;
        u32 shamt;
    };

    // Fence instruction.
    struct FenceType
    {
        u32 fm;
        Reg rd;
        Reg rs1;
    };

    struct Encoding
    {
        Opcode opcode;
        union
        {
            IllegalType illegal;
            BType btype;
            IType itype;
            SType stype;
            UType utype;
            JType jtype;
            ArithType arithType;
            ImmShiftType immShiftType;
            FenceType fenceType;
        };
    };

    // An Rv32i instruction handler that re-encodes instructions for Arviss.
    class Rv32iArvissEncoder
    {
    public:
        using Item = Encoding;

        // Illegal instruction.
        auto Illegal(u32 ins) -> Item { return {.opcode = Opcode::Illegal, .illegal = {.ins = ins}}; }

        // B-type instructions.
        auto Beq(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.opcode = Opcode::Beq, .btype = {.rs1 = rs1, .rs2 = rs2, .bimm = bimm}}; }
        auto Bne(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.opcode = Opcode::Bne, .btype = {.rs1 = rs1, .rs2 = rs2, .bimm = bimm}}; }
        auto Blt(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.opcode = Opcode::Blt, .btype = {.rs1 = rs1, .rs2 = rs2, .bimm = bimm}}; }
        auto Bge(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.opcode = Opcode::Bge, .btype = {.rs1 = rs1, .rs2 = rs2, .bimm = bimm}}; }
        auto Bltu(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.opcode = Opcode::Bltu, .btype = {.rs1 = rs1, .rs2 = rs2, .bimm = bimm}}; }
        auto Bgeu(Reg rs1, Reg rs2, u32 bimm) -> Item { return {.opcode = Opcode::Bgeu, .btype = {.rs1 = rs1, .rs2 = rs2, .bimm = bimm}}; }

        // I-type instructions.
        auto Lb(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Lb, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Lh(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Lh, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Lw(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Lw, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Lbu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Lbu, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Lhu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Lhu, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Addi(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Addi, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Slti(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Slti, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Sltiu(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Sltiu, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Xori(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Xori, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Ori(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Ori, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Andi(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Andi, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }
        auto Jalr(Reg rd, Reg rs1, u32 iimm) -> Item { return {.opcode = Opcode::Jalr, .itype = {.rd = rd, .rs1 = rs1, .iimm = iimm}}; }

        // S-type instructions.
        auto Sb(Reg rs1, Reg rs2, u32 simm) -> Item { return {.opcode = Opcode::Sb, .stype = {.rs1 = rs1, .rs2 = rs2, .simm = simm}}; }
        auto Sh(Reg rs1, Reg rs2, u32 simm) -> Item { return {.opcode = Opcode::Sh, .stype = {.rs1 = rs1, .rs2 = rs2, .simm = simm}}; }
        auto Sw(Reg rs1, Reg rs2, u32 simm) -> Item { return {.opcode = Opcode::Sw, .stype = {.rs1 = rs1, .rs2 = rs2, .simm = simm}}; }

        // U-type instructions.
        auto Auipc(Reg rd, u32 uimm) -> Item { return {.opcode = Opcode::Auipc, .utype = {.rd = rd, .uimm = uimm}}; }
        auto Lui(Reg rd, u32 uimm) -> Item { return {.opcode = Opcode::Lui, .utype = {.rd = rd, .uimm = uimm}}; }

        // J-type instructions.
        auto Jal(Reg rd, u32 jimm) -> Item { return {.opcode = Opcode::Jal, .jtype = {.rd = rd, .jimm = jimm}}; }

        // Arithmetic instructions.
        auto Add(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Add, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Sub(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Sub, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Sll(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Sll, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Slt(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Slt, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Sltu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Sltu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Xor(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Xor, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Srl(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Srl, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Sra(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Sra, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Or(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Or, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto And(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::And, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }

        // Immediate shift instructions.

        auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item { return {.opcode = Opcode::Slli, .immShiftType{.rd = rd, .rs1 = rs1, .shamt = shamt}}; }
        auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item { return {.opcode = Opcode::Srli, .immShiftType{.rd = rd, .rs1 = rs1, .shamt = shamt}}; }
        auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item { return {.opcode = Opcode::Srai, .immShiftType{.rd = rd, .rs1 = rs1, .shamt = shamt}}; }

        auto Fence(u32 fm, Reg rd, Reg rs1) -> Item { return {.opcode = Opcode::Fence, .fenceType = {.fm = fm, .rd = rd, .rs1 = rs1}}; }
        auto Ecall() -> Item { return {.opcode = Opcode::Ecall}; }
        auto Ebreak() -> Item { return {.opcode = Opcode::Ebreak}; }
    };

    static_assert(IsRv32iInstructionHandler<Rv32iArvissEncoder>);

    // An Rv32im instruction handler that re-encodes instructions for Arviss.
    class Rv32imArvissEncoder : public Rv32iArvissEncoder
    {
    public:
        using Item = Rv32iArvissEncoder::Item;

        auto Mul(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Mul, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Mulh(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Mulh, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Mulhsu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Mulhsu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Mulhu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Mulhu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Div(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Div, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Divu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Divu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Rem(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Rem, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
        auto Remu(Reg rd, Reg rs1, Reg rs2) -> Item { return {.opcode = Opcode::Remu, .arithType = {.rd = rd, .rs1 = rs1, .rs2 = rs2}}; }
    };

    static_assert(IsRv32imInstructionHandler<Rv32imArvissEncoder>);

} // namespace arviss
