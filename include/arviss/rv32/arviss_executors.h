#pragma once

#include "arviss/core/concepts.h"
#include "arviss/rv32/arviss_encoder.h"
#include "arviss/rv32/concepts.h"
#include "arviss/rv32/executors.h"

namespace arviss
{
    template<typename T>
    concept IsCache = requires(T t, Encoding e) {
        e = t.Get(Address{}); // Read from the cache at a given address.
        t.Put(Address{}, e);  // Write to the cache at a given address.
    };

    class SimpleCache
    {
        static const size_t defaultCacheSize = 8192;

        std::vector<Encoding> cache_ = std::vector<Encoding>(defaultCacheSize);

    public:
        SimpleCache() = default;
        SimpleCache(size_t size) : cache_(size) {}

        Encoding Get(Address addr) { return cache_[addr]; }
        void Put(Address addr, Encoding e) { cache_[addr] = e; }
    };

    template<IsRv32iCpu T, IsCache CacheT>
    class Arviss32iDispatcher : public T
    {
        CacheT cache_;
        Rv32iDispatcher<Rv32iArvissEncoder> encoder_{};
        Address pc_{};

        auto Self() -> T& { return static_cast<T&>(*this); }

    public:
        using Item = typename T::Item;

        Arviss32iDispatcher() : Arviss32iDispatcher(CacheT()) {}

        Arviss32iDispatcher(CacheT&& cache) : cache_{std::move(cache)} {}

        auto QuickDispatch() -> Item
        {
            auto& self = Self();
            pc_ = self.Transfer();                          // Update pc from nextPc.
            const auto arvissEncoded = cache_.Get(pc_ / 4); // Look for the instruction in the cache. It'll be an Fdx if not present.
            self.SetNextPc(pc_ + 4);                        // Go to the next instruction.
            return DispatchEncoded(arvissEncoded);          // Dispatch the Arviss-encoded instruction.
        }

        auto DispatchEncoded(const Encoding& e) -> Item
        {
            auto& self = Self();

            switch (e.opcode)
            {
            // --- Arviss.
            case Opcode::Fdx: {
                auto ins = self.Fetch32(pc_);                // Fetch the RISC-V encoded instruction from memory.
                auto arvissEncoded = encoder_.Dispatch(ins); // Encode it for Arviss.
                cache_.Put(pc_ / 4, arvissEncoded);          // Cache it.

                // Doesn't recurse, because we'll get an illegal instruction for anything that the encoder didn't know about.
                return DispatchEncoded(arvissEncoded);
            }

            // --- RV32i.
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

            // --- RV32m.
            // Integer multiply and divide instructions.
            case Opcode::Mul:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Mul(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Mulh:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Mulh(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Mulhsu:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Mulhsu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Mulhu:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Mulhu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Div:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Div(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Divu:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Divu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Rem:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Rem(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }
            case Opcode::Remu:
                if constexpr (IsRv32imCpu<T>)
                {
                    return self.Remu(e.arithType.rd, e.arithType.rs1, e.arithType.rs2);
                }

            // --- RV32f.
            // Floating point instructions.
            case Opcode::Fmv_x_w:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmv_x_w(e.floatRdRs1.rd, e.floatRdRs1.rs1);
                }
            case Opcode::Fclass_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fclass_s(e.floatRdRs1.rd, e.floatRdRs1.rs1);
                }
            case Opcode::Fmv_w_x:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmv_w_x(e.floatRdRs1.rd, e.floatRdRs1.rs1);
                }
            case Opcode::Fsqrt_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fsqrt_s(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
                }
            case Opcode::Fcvt_w_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fcvt_w_s(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
                }
            case Opcode::Fcvt_wu_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fcvt_wu_s(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
                }
            case Opcode::Fcvt_s_w:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fcvt_s_w(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
                }
            case Opcode::Fcvt_s_wu:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fcvt_s_wu(e.floatRdRs1Rm.rd, e.floatRdRs1Rm.rs1, e.floatRdRs1Rm.rm);
                }
            case Opcode::Fsgnj_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fsgnj_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Fsgnjn_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fsgnjn_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Fsgnjx_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fsgnjx_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Fmin_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmin_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Fmax_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmax_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Fle_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fle_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Flt_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Flt_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Feq_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Feq_s(e.floatRdRs1Rs2.rd, e.floatRdRs1Rs2.rs1, e.floatRdRs1Rs2.rs2);
                }
            case Opcode::Fadd_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fadd_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
                }
            case Opcode::Fsub_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fsub_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
                }
            case Opcode::Fmul_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmul_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
                }
            case Opcode::Fdiv_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fdiv_s(e.floatRdRs1Rs2Rm.rd, e.floatRdRs1Rs2Rm.rs1, e.floatRdRs1Rs2Rm.rs2, e.floatRdRs1Rs2Rm.rm);
                }
            case Opcode::Flw:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Flw(e.floatRdRs1Imm.rd, e.floatRdRs1Imm.rs1, e.floatRdRs1Imm.imm);
                }
            case Opcode::Fsw:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fsw(e.floatRs1Rs2Imm.rs1, e.floatRs1Rs2Imm.rs2, e.floatRs1Rs2Imm.imm);
                }
            case Opcode::Fmadd_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmadd_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
                                        e.floatRdRs1Rs2Rs3Rm.rm);
                }
            case Opcode::Fmsub_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fmsub_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
                                        e.floatRdRs1Rs2Rs3Rm.rm);
                }
            case Opcode::Fnmsub_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fnmsub_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
                                         e.floatRdRs1Rs2Rs3Rm.rm);
                }
            case Opcode::Fnmadd_s:
                if constexpr (IsRv32imfCpu<T>)
                {
                    return self.Fnmadd_s(e.floatRdRs1Rs2Rs3Rm.rd, e.floatRdRs1Rs2Rs3Rm.rs1, e.floatRdRs1Rs2Rs3Rm.rs2, e.floatRdRs1Rs2Rs3Rm.rs3,
                                         e.floatRdRs1Rs2Rs3Rm.rm);
                }

            // Default to an illegal instruction.
            default:
                return self.Illegal(e.illegal.ins);
            }
        }
    };
} // namespace arviss
