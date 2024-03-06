#pragma once

#include "arviss/common/types.h"
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
        auto ConverterFor() -> Rv32iDispatcher<Rv32iToRemixConverter>;

        template<typename T>
            requires IsRv32iInstructionHandler<T>  // T is a handler for Rv32i.
                && IsRv32mInstructionHandler<T>    // T is a handler for Rv32m.
                && (!IsRv32fInstructionHandler<T>) // T is NOT a handler for Rv32f.
        auto ConverterFor() -> Rv32imDispatcher<Rv32imToRemixConverter>;

        template<typename T>
            requires IsRv32iInstructionHandler<T> // T is a handler for Rv32i.
                && IsRv32mInstructionHandler<T>   // T is a handler for Rv32i.
                && IsRv32fInstructionHandler<T>   // T is a handler for Rv32f.
        auto ConverterFor() -> Rv32imfDispatcher<Rv32imfToRemixConverter>;

    } // namespace

    template<IsRemixDispatchable T>
    class RemixDispatcher : public T
    {
        auto Self() -> T& { return static_cast<T&>(*this); }

        // We want a different converter dependending on the capabilities of the instruction handler, because otherwise
        // its dispatcher has to do work unnecessarily.
        using ConverterType = decltype(ConverterFor<T>());

        ConverterType converter_{};

    public:
        using Item = typename T::Item;

        auto Transcode(u32 code) -> Item
        {
            auto& self = Self();
            const auto remixed = converter_.Dispatch(code);
            if (remixed.f0.opc() == Opcode::Illegal)
            {
                return self.Illegal(code);
            }
            const u32 recode = *reinterpret_cast<const u32*>(&remixed);
            self.Write32Unprotected(self.Pc(), recode);
            return Dispatch(recode);
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

            // --- RV32m.

            // Integer multiply and divide instructions.
            case Opcode::Mul:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Mul(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Mulh:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Mulh(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Mulhsu:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Mulhsu(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Mulhu:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Mulhu(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Div:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Div(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Divu:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Divu(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Rem:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Rem(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            case Opcode::Remu:
                if constexpr (IsRv32mInstructionHandler<T>)
                {
                    return self.Remu(e.arithType.rd(), e.arithType.rs1(), e.arithType.rs2());
                }
                [[fallthrough]];
            // --- RV32f.

            // Floating point instructions.
            case Opcode::Fmv_x_w:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmv_x_w(e.f5Type.rd(), e.f5Type.rs1());
                }
                [[fallthrough]];
            case Opcode::Fclass_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fclass_s(e.f5Type.rd(), e.f5Type.rs1());
                }
                [[fallthrough]];
            case Opcode::Fmv_w_x:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmv_w_x(e.f5Type.rd(), e.f5Type.rs1());
                }
                [[fallthrough]];
            case Opcode::Fsqrt_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fsqrt_s(e.f5rmType.rd(), e.f5rmType.rs1(), e.f5rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fcvt_w_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fcvt_w_s(e.f5rmType.rd(), e.f5rmType.rs1(), e.f5rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fcvt_wu_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fcvt_wu_s(e.f5rmType.rd(), e.f5rmType.rs1(), e.f5rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fcvt_s_w:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fcvt_s_w(e.f5rmType.rd(), e.f5rmType.rs1(), e.f5rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fcvt_s_wu:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fcvt_s_wu(e.f5rmType.rd(), e.f5rmType.rs1(), e.f5rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fsgnj_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fsgnj_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Fsgnjn_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fsgnjn_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Fsgnjx_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fsgnjx_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Fmin_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmin_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Fmax_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmax_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Fle_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fle_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Flt_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Flt_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Feq_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Feq_s(e.f6Type.rd(), e.f6Type.rs1(), e.f6Type.rs2());
                }
                [[fallthrough]];
            case Opcode::Fadd_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fadd_s(e.f6rmType.rd(), e.f6rmType.rs1(), e.f6rmType.rs2(), e.f6rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fsub_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fsub_s(e.f6rmType.rd(), e.f6rmType.rs1(), e.f6rmType.rs2(), e.f6rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fmul_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmul_s(e.f6rmType.rd(), e.f6rmType.rs1(), e.f6rmType.rs2(), e.f6rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Fdiv_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fdiv_s(e.f6rmType.rd(), e.f6rmType.rs1(), e.f6rmType.rs2(), e.f6rmType.rm());
                }
                [[fallthrough]];
            case Opcode::Flw:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Flw(e.itype.rd(), e.itype.rs1(), e.itype.iimm());
                }
                [[fallthrough]];
            case Opcode::Fsw:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fsw(e.stype.rs1(), e.stype.rs2(), e.stype.simm());
                }
                [[fallthrough]];
            case Opcode::Fmadd_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmadd_s(e.f7Type.rd(), e.f7Type.rs1(), e.f7Type.rs2(), e.f7Type.rs3(), e.f7Type.rm());
                }
                [[fallthrough]];
            case Opcode::Fmsub_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fmsub_s(e.f7Type.rd(), e.f7Type.rs1(), e.f7Type.rs2(), e.f7Type.rs3(), e.f7Type.rm());
                }
                [[fallthrough]];
            case Opcode::Fnmsub_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fnmsub_s(e.f7Type.rd(), e.f7Type.rs1(), e.f7Type.rs2(), e.f7Type.rs3(), e.f7Type.rm());
                }
                [[fallthrough]];
            case Opcode::Fnmadd_s:
                if constexpr (IsRv32fInstructionHandler<T>)
                {
                    return self.Fnmadd_s(e.f7Type.rd(), e.f7Type.rs1(), e.f7Type.rs2(), e.f7Type.rs3(), e.f7Type.rm());
                }
                [[fallthrough]];

            // If we don't know it then we assume it's RISC-V encoded and try to transcode it.
            default:
                return Transcode(code);
            }
        }
    };
} // namespace arviss::remix
