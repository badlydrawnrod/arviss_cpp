#pragma once

#include "arviss/core/concepts.h"
#include "arviss/rv32/arviss_encoder.h" // Opcode.
#include "arviss/rv32/concepts.h"
#include "arviss/rv32/executors.h"

namespace arviss
{
    template<IsRv32iInstructionHandler T>
    class Arviss32iDispatcher : public T
    {
        auto Self() -> T& { return static_cast<T&>(*this); }

    public:
        using Item = typename T::Item;

        auto DispatchEncoded(const Encoding& e) -> Item
        {
            auto& self = Self();

            switch (e.opcode)
            {
            // Arviss.
            case Opcode::Fdx:
                // TODO: implement Fdx.
                break;

            // --- RV32i

            // Illegal instruction.
            case Opcode::Illegal:
                return self.Illegal(e.illegal.ins);

            // B-type instructions.
            case Opcode::Beq:
                return self.Beq(e.btype.rs1, e.btype.rs2, e.btype.bimm);
            case Opcode::Bne:
                return self.Bne(e.btype.rs1, e.btype.rs2, e.btype.bimm);
            case Opcode::Blt:
                return self.Blt(e.btype.rs1, e.btype.rs2, e.btype.bimm);
            case Opcode::Bge:
                return self.Bge(e.btype.rs1, e.btype.rs2, e.btype.bimm);
            case Opcode::Bltu:
                return self.Bltu(e.btype.rs1, e.btype.rs2, e.btype.bimm);
            case Opcode::Bgeu:
                return self.Bgeu(e.btype.rs1, e.btype.rs2, e.btype.bimm);

            // I-type instructions.
            case Opcode::Lb:
                return self.Lb(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Lh:
                return self.Lh(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Lw:
                return self.Lw(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Lbu:
                return self.Lbu(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Lhu:
                return self.Lhu(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Addi:
                return self.Addi(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Slti:
                return self.Slti(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Sltiu:
                return self.Sltiu(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Xori:
                return self.Xori(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Ori:
                return self.Ori(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Andi:
                return self.Andi(e.itype.rd, e.itype.rs1, e.itype.iimm);
            case Opcode::Jalr:
                return self.Jalr(e.itype.rd, e.itype.rs1, e.itype.iimm);

            // S-type instructions.
            case Opcode::Sb:
                return self.Sb(e.stype.rs1, e.stype.rs2, e.stype.simm);
            case Opcode::Sh:
                return self.Sh(e.stype.rs1, e.stype.rs2, e.stype.simm);
            case Opcode::Sw:
                return self.Sw(e.stype.rs1, e.stype.rs2, e.stype.simm);

            // U-type instructions.
            case Opcode::Auipc:
                return self.Auipc(e.utype.rd, e.utype.uimm);
            case Opcode::Lui:
                return self.Lui(e.utype.rd, e.utype.uimm);

            // J-type instructions.
            case Opcode::Jal:
                return self.Jal(e.jtype.rd, e.jtype.jimm);

            // Arithmetic instructions.
            case Opcode::Add:
                return self.Add(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Sub:
                return self.Sub(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Sll:
                return self.Sll(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Slt:
                return self.Slt(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Sltu:
                return self.Sltu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Xor:
                return self.Xor(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Srl:
                return self.Srl(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Sra:
                return self.Sra(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::Or:
                return self.Or(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            case Opcode::And:
                return self.And(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);

            // Immediate shift instructions.
            case Opcode::Slli:
                return self.Slli(e.immShiftType.rd, e.immShiftType.rs1, e.immShiftType.shamt);
            case Opcode::Srli:
                return self.Srli(e.immShiftType.rd, e.immShiftType.rs1, e.immShiftType.shamt);
            case Opcode::Srai:
                return self.Srai(e.immShiftType.rd, e.immShiftType.rs1, e.immShiftType.shamt);

            // System instructions.
            case Opcode::Fence:
                return self.Fence(e.fenceType.fm, e.fenceType.rd, e.fenceType.rs1);
            case Opcode::Ecall:
                return self.Ecall();
            case Opcode::Ebreak:
                return self.Ebreak();
            }

            // Default to an illegal instruction.
            return self.Illegal(e.illegal.ins);
        }
    };
} // namespace arviss
