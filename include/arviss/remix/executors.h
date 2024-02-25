#pragma once

#include "arviss/core/concepts.h"
#include "arviss/remix/concepts.h"
#include "arviss/remix/encoder.h"
#include "arviss/rv32/concepts.h"
#include "arviss/rv32/dispatchers.h"
#include "arviss/rv32/executors.h"

namespace arviss::remix
{
    namespace
    {
        template<typename T>
            requires IsRv32iInstructionHandler<T>  // T is a handler for Rv32i.
                && (!IsRv32mInstructionHandler<T>) // T is NOT a handler for Rv32m.
                && (!IsRv32fInstructionHandler<T>) // T is NOT a handler for Rv32f.
        auto EncoderFor() -> Rv32iDispatcher<Rv32iToRemixConverter>;

        // template<typename T>
        //     requires IsRv32iInstructionHandler<T>  // T is a handler for Rv32i.
        //         && IsRv32mInstructionHandler<T>    // T is a handler for Rv32m.
        //         && (!IsRv32fInstructionHandler<T>) // T is NOT a handler for Rv32f.
        // auto EncoderFor() -> Rv32imDispatcher<Rv32imToRemixConverter>;

        // template<typename T>
        //     requires IsRv32iInstructionHandler<T> // T is a handler for Rv32i.
        //         && IsRv32mInstructionHandler<T>   // T is a handler for Rv32i.
        //         && IsRv32fInstructionHandler<T>   // T is a handler for Rv32f.
        // auto EncoderFor() -> Rv32imfDispatcher<Rv32imfToRemixConverter>;

    } // namespace

    template<IsRemixDispatchable T>
    class RemixDispatcher : public T
    {
        auto Self() -> T& { return static_cast<T&>(*this); }

        // TODO: support multiple converter types.
        Rv32iDispatcher<Rv32iToRemixConverter> converter_;

    public:
        using Item = typename T::Item;

        auto Transcode(u32 code) -> void
        {
            auto remixed = converter_.Dispatch(code);
            u32 recode = *reinterpret_cast<u32*>(&remixed);
            auto& self = Self();
            self.Write32(self.Pc(), recode);
            Dispatch(recode);
        }

        auto Dispatch(u32 code) -> Item
        {
            auto& self = Self();
            const Remix e = *reinterpret_cast<Remix*>(&code);

            switch (e.f0.opc())
            {
            // Illegal instruction.
            case Opcode::Illegal:
                return self.Illegal(code);

            // --- RISC-V opcodes (not remixed).
            case Opcode::Rv03:
            case Opcode::Rv07:
            case Opcode::Rv0b:
            case Opcode::Rv0f:
            case Opcode::Rv13:
            case Opcode::Rv17:
            case Opcode::Rv1b:
            case Opcode::Rv1f:
            case Opcode::Rv23:
            case Opcode::Rv27:
            case Opcode::Rv2b:
            case Opcode::Rv2f:
            case Opcode::Rv33:
            case Opcode::Rv37:
            case Opcode::Rv3b:
            case Opcode::Rv3f:
                Transcode(code);
                return;

            // --- RV32i.

            // B-type instructions.
            case Opcode::Beq:
                return self.Beq(e.btype.rs1(), e.btype.rs2(), e.btype.bimm());
            case Opcode::Bne:
                return self.Bne(e.btype.rs1(), e.btype.rs2(), e.btype.bimm());
            case Opcode::Blt:
                return self.Blt(e.btype.rs1(), e.btype.rs2(), e.btype.bimm());
            case Opcode::Bge:
                return self.Bge(e.btype.rs1(), e.btype.rs2(), e.btype.bimm());
            case Opcode::Bltu:
                return self.Bltu(e.btype.rs1(), e.btype.rs2(), e.btype.bimm());
            case Opcode::Bgeu:
                return self.Bgeu(e.btype.rs1(), e.btype.rs2(), e.btype.bimm());

            // I-type instructions.
            case Opcode::Lb:
                return self.Lb(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Lh:
                return self.Lh(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Lw:
                return self.Lw(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Lbu:
                return self.Lbu(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Lhu:
                return self.Lhu(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Addi:
                return self.Addi(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Slti:
                return self.Slti(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Sltiu:
                return self.Sltiu(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Xori:
                return self.Xori(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Ori:
                return self.Ori(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Andi:
                return self.Andi(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
            case Opcode::Jalr:
                return self.Jalr(e.itype.rd(), e.itype.rs1(), e.itype.iimm());

            // S-type instructions.
            case Opcode::Sb:
                return self.Sb(e.stype.rs1(), e.stype.rs2(), e.stype.simm());
            case Opcode::Sh:
                return self.Sh(e.stype.rs1(), e.stype.rs2(), e.stype.simm());
            case Opcode::Sw:
                return self.Sw(e.stype.rs1(), e.stype.rs2(), e.stype.simm());

            // U-type instructions.
            case Opcode::Auipc:
                return self.Auipc(e.utype.rd(), e.utype.uimm());
            case Opcode::Lui:
                return self.Lui(e.utype.rd(), e.utype.uimm());

            // J-type instructions.
            case Opcode::Jal:
                return self.Jal(e.jtype.rd(), e.jtype.jimm());

            // Arithmetic instructions.
            case Opcode::Add:
                return self.Add(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Sub:
                return self.Sub(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Sll:
                return self.Sll(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Slt:
                return self.Slt(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Sltu:
                return self.Sltu(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Xor:
                return self.Xor(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Srl:
                return self.Srl(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Sra:
                return self.Sra(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::Or:
                return self.Or(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
            case Opcode::And:
                return self.And(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());

            // Immediate shift instructions.
            case Opcode::Slli:
                return self.Slli(e.immShiftType.rd(), e.immShiftType.rs1(), e.immShiftType.shamt());
            case Opcode::Srli:
                return self.Srli(e.immShiftType.rd(), e.immShiftType.rs1(), e.immShiftType.shamt());
            case Opcode::Srai:
                return self.Srai(e.immShiftType.rd(), e.immShiftType.rs1(), e.immShiftType.shamt());

            // System instructions.
            case Opcode::Fence:
                return self.Fence(e.fenceType.fm(), e.fenceType.rd(), e.fenceType.rs1());
            case Opcode::Ecall:
                return self.Ecall();
            case Opcode::Ebreak:
                return self.Ebreak();

            // // --- RV32m.
            // // Integer multiply and divide instructions.
            // case Opcode::Mul:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Mul(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Mulh:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Mulh(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Mulhsu:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Mulhsu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Mulhu:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Mulhu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Div:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Div(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Divu:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Divu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Rem:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Rem(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }
            // case Opcode::Remu:
            //     if constexpr (IsRv32mInstructionHandler<T>)
            //     {
            //         self.Remu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
            //     }

            // // --- RV32f.
            // // Floating point instructions.
            // case Opcode::Fmv_x_w:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmv_x_w(e.floatRdRs1.rd, e.floatRdRs1.rs1);
            //     }
            // case Opcode::Fclass_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fclass_s(e.floatRdRs1.rd, e.floatRdRs1.rs1);
            //     }
            // case Opcode::Fmv_w_x:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmv_w_x(e.floatRdRs1.rd, e.floatRdRs1.rs1);
            //     }
            // case Opcode::Fsqrt_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fsqrt_s(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
            //     }
            // case Opcode::Fcvt_w_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fcvt_w_s(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
            //     }
            // case Opcode::Fcvt_wu_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fcvt_wu_s(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
            //     }
            // case Opcode::Fcvt_s_w:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fcvt_s_w(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
            //     }
            // case Opcode::Fcvt_s_wu:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fcvt_s_wu(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
            //     }
            // case Opcode::Fsgnj_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fsgnj_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Fsgnjn_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fsgnjn_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Fsgnjx_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fsgnjx_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Fmin_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmin_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Fmax_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmax_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Fle_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fle_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Flt_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Flt_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Feq_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Feq_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
            //     }
            // case Opcode::Fadd_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fadd_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
            //     }
            // case Opcode::Fsub_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fsub_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
            //     }
            // case Opcode::Fmul_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmul_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
            //     }
            // case Opcode::Fdiv_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fdiv_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
            //     }
            // case Opcode::Flw:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Flw(e.floatRdRs1Imm.rd, e.floatRdRs1Imm.rs1, e.floatRdRs1Imm.imm);
            //     }
            // case Opcode::Fsw:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fsw(e.floatRs1Rs2Imm.rs1, e.floatRs1Rs2Imm.rs2, e.floatRs1Rs2Imm.imm);
            //     }
            // case Opcode::Fmadd_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmadd_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
            //                             e.floatRdRs1Rs2Rs3Rm.rm);
            //     }
            // case Opcode::Fmsub_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fmsub_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
            //                             e.floatRdRs1Rs2Rs3Rm.rm);
            //     }
            // case Opcode::Fnmsub_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fnmsub_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
            //                              e.floatRdRs1Rs2Rs3Rm.rm);
            //     }
            // case Opcode::Fnmadd_s:
            //     if constexpr (IsRv32fInstructionHandler<T>)
            //     {
            //         self.Fnmadd_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
            //                              e.floatRdRs1Rs2Rs3Rm.rm);
            //     }
            default:
                Transcode(code);
                return;
            }
        }
    };
} // namespace arviss::remix
