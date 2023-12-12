#pragma once

#include "arviss/core/concepts.h"
#include "arviss/instruction.h"
#include "arviss/rv32_concepts.h"

namespace arviss
{
    // This code was generated by `tools/make_dispatcher.py c++`. Do not edit.

    // A dispatcher for RV32I instructions. BYO handler.
    template<typename Handler>
        requires IsRv32iInstructionHandler<Handler>
    struct Rv32iDispatcher : public Handler
    {
        using Item = Handler::Item;

        // Decodes the input word to an RV32I instruction and dispatches it to a handler.
        // clang-format off
        auto Dispatch(u32 code) -> Item
        {
            Handler& self = static_cast<Handler&>(*this);
            Instruction c(code);

            switch (code) {
                case 0x00000073: return self.Ecall();
                case 0x00100073: return self.Ebreak();
            }
            switch (code & 0xfe00707f) {
                case 0x00000033: return self.Add(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40000033: return self.Sub(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001033: return self.Sll(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00002033: return self.Slt(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00003033: return self.Sltu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00004033: return self.Xor(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00005033: return self.Srl(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40005033: return self.Sra(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00006033: return self.Or(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00007033: return self.And(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001013: return self.Slli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x00005013: return self.Srli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x40005013: return self.Srai(c.Rd(), c.Rs1(), c.Shamtw());
            }
            switch (code & 0x0000707f) {
                case 0x00000063: return self.Beq(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00001063: return self.Bne(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00004063: return self.Blt(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00005063: return self.Bge(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00006063: return self.Bltu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00007063: return self.Bgeu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00000067: return self.Jalr(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000013: return self.Addi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002013: return self.Slti(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00003013: return self.Sltiu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004013: return self.Xori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00006013: return self.Ori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00007013: return self.Andi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000003: return self.Lb(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00001003: return self.Lh(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002003: return self.Lw(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004003: return self.Lbu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00005003: return self.Lhu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000023: return self.Sb(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00001023: return self.Sh(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00002023: return self.Sw(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x0000000f: return self.Fence(c.Fm(), c.Rd(), c.Rs1());
            }
            switch (code & 0x0000007f) {
                case 0x0000006f: return self.Jal(c.Rd(), c.Jimmediate());
                case 0x00000037: return self.Lui(c.Rd(), c.Uimmediate());
                case 0x00000017: return self.Auipc(c.Rd(), c.Uimmediate());
            }
            return self.Illegal(code);
        }
        // clang-format on
    };

    // End of auto-generated code.

    // This code was generated by `tools/make_dispatcher.py -c c++`. Do not edit.

    // A dispatcher for RV32IC instructions. BYO handler.
    template<typename Handler>
        requires IsRv32iInstructionHandler<Handler> && IsRv32cInstructionHandler<Handler>
    struct Rv32icDispatcher : public Handler
    {
        using Item = Handler::Item;

        // Decodes the input word to an RV32IC instruction and dispatches it to a handler.
        // clang-format off
        auto Dispatch(u32 code) -> Item
        {
            Handler& self = static_cast<Handler&>(*this);
            Instruction c(code);

            switch (code) {
                case 0x00000073: return self.Ecall();
                case 0x00100073: return self.Ebreak();
            }
            switch (code & 0xfe00707f) {
                case 0x00000033: return self.Add(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40000033: return self.Sub(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001033: return self.Sll(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00002033: return self.Slt(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00003033: return self.Sltu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00004033: return self.Xor(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00005033: return self.Srl(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40005033: return self.Sra(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00006033: return self.Or(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00007033: return self.And(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001013: return self.Slli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x00005013: return self.Srli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x40005013: return self.Srai(c.Rd(), c.Rs1(), c.Shamtw());
            }
            switch (code & 0x0000ffff) {
                case 0x9002: return self.C_ebreak();
            }
            switch (code & 0x0000f07f) {
                case 0x8002: return self.C_jr(c.Rs1n0());
                case 0x9002: return self.C_jalr(c.Rs1n0());
            }
            switch (code & 0x0000707f) {
                case 0x00000063: return self.Beq(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00001063: return self.Bne(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00004063: return self.Blt(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00005063: return self.Bge(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00006063: return self.Bltu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00007063: return self.Bgeu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00000067: return self.Jalr(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000013: return self.Addi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002013: return self.Slti(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00003013: return self.Sltiu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004013: return self.Xori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00006013: return self.Ori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00007013: return self.Andi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000003: return self.Lb(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00001003: return self.Lh(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002003: return self.Lw(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004003: return self.Lbu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00005003: return self.Lhu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000023: return self.Sb(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00001023: return self.Sh(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00002023: return self.Sw(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x0000000f: return self.Fence(c.Fm(), c.Rd(), c.Rs1());
            }
            switch (code & 0x0000ef83) {
                case 0x0001: return self.C_nop(c.C_nzimm6());
                case 0x6101: return self.C_addi16sp(c.C_nzimm10());
            }
            switch (code & 0x0000fc63) {
                case 0x8c01: return self.C_sub(c.Rdrs1p(), c.Rs2p());
                case 0x8c21: return self.C_xor(c.Rdrs1p(), c.Rs2p());
                case 0x8c41: return self.C_or(c.Rdrs1p(), c.Rs2p());
                case 0x8c61: return self.C_and(c.Rdrs1p(), c.Rs2p());
            }
            switch (code & 0x0000007f) {
                case 0x0000006f: return self.Jal(c.Rd(), c.Jimmediate());
                case 0x00000037: return self.Lui(c.Rd(), c.Uimmediate());
                case 0x00000017: return self.Auipc(c.Rd(), c.Uimmediate());
            }
            switch (code & 0x0000ec03) {
                case 0x8801: return self.C_andi(c.Rdrs1p(), c.C_imm6());
                case 0x8001: return self.C_srli(c.Rdrs1p(), c.C_nzuimm6());
                case 0x8401: return self.C_srai(c.Rdrs1p(), c.C_nzuimm6());
            }
            switch (code & 0x0000f003) {
                case 0x8002: return self.C_mv(c.Rd(), c.Rs2n0());
                case 0x9002: return self.C_add(c.Rdrs1(), c.Rs2n0());
            }
            switch (code & 0x0000e003) {
                case 0x0000: return self.C_addi4spn(c.Rdp(), c.C_nzuimm10());
                case 0x4000: return self.C_lw(c.Rdp(), c.Rs1p(), c.C_uimm7());
                case 0xc000: return self.C_sw(c.Rs1p(), c.Rs2p(), c.C_uimm7());
                case 0x0001: return self.C_addi(c.Rdrs1n0(), c.C_nzimm6());
                case 0x4001: return self.C_li(c.Rd(), c.C_imm6());
                case 0x6001: return self.C_lui(c.Rdn2(), c.C_nzimm18());
                case 0xa001: return self.C_j(c.C_imm12());
                case 0xc001: return self.C_beqz(c.Rs1p(), c.C_bimm9());
                case 0xe001: return self.C_bnez(c.Rs1p(), c.C_bimm9());
                case 0x4002: return self.C_lwsp(c.Rdn0(), c.C_uimm8sp());
                case 0xc002: return self.C_swsp(c.C_rs2(), c.C_uimm8sp_s());
                case 0x2001: return self.C_jal(c.C_imm12());
                case 0x0002: return self.C_slli(c.Rdrs1n0(), c.C_nzuimm6());
            }
            return self.Illegal(code);
        }
        // clang-format on
    };

    // End of auto-generated code.

    // This code was generated by `tools/make_dispatcher.py -m c++`. Do not edit.

    // A dispatcher for RV32IM instructions. BYO handler.
    template<typename Handler>
        requires IsRv32iInstructionHandler<Handler> && IsRv32mInstructionHandler<Handler>
    struct Rv32imDispatcher : public Handler
    {
        using Item = Handler::Item;

        // Decodes the input word to an RV32IM instruction and dispatches it to a handler.
        // clang-format off
        auto Dispatch(u32 code) -> Item
        {
            Handler& self = static_cast<Handler&>(*this);
            Instruction c(code);

            switch (code) {
                case 0x00000073: return self.Ecall();
                case 0x00100073: return self.Ebreak();
            }
            switch (code & 0xfe00707f) {
                case 0x00000033: return self.Add(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40000033: return self.Sub(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001033: return self.Sll(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00002033: return self.Slt(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00003033: return self.Sltu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00004033: return self.Xor(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00005033: return self.Srl(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40005033: return self.Sra(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00006033: return self.Or(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00007033: return self.And(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001013: return self.Slli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x00005013: return self.Srli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x40005013: return self.Srai(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x02000033: return self.Mul(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02001033: return self.Mulh(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02002033: return self.Mulhsu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02003033: return self.Mulhu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02004033: return self.Div(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02005033: return self.Divu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02006033: return self.Rem(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02007033: return self.Remu(c.Rd(), c.Rs1(), c.Rs2());
            }
            switch (code & 0x0000707f) {
                case 0x00000063: return self.Beq(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00001063: return self.Bne(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00004063: return self.Blt(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00005063: return self.Bge(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00006063: return self.Bltu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00007063: return self.Bgeu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00000067: return self.Jalr(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000013: return self.Addi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002013: return self.Slti(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00003013: return self.Sltiu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004013: return self.Xori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00006013: return self.Ori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00007013: return self.Andi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000003: return self.Lb(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00001003: return self.Lh(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002003: return self.Lw(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004003: return self.Lbu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00005003: return self.Lhu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000023: return self.Sb(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00001023: return self.Sh(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00002023: return self.Sw(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x0000000f: return self.Fence(c.Fm(), c.Rd(), c.Rs1());
            }
            switch (code & 0x0000007f) {
                case 0x0000006f: return self.Jal(c.Rd(), c.Jimmediate());
                case 0x00000037: return self.Lui(c.Rd(), c.Uimmediate());
                case 0x00000017: return self.Auipc(c.Rd(), c.Uimmediate());
            }
            return self.Illegal(code);
        }
        // clang-format on
    };

    // End of auto-generated code.

    // This code was generated by `tools/make_dispatcher.py -mf c++`. Do not edit.

    // A dispatcher for RV32IMF instructions. BYO handler.
    template<typename Handler>
        requires IsRv32iInstructionHandler<Handler> && IsRv32mInstructionHandler<Handler> && IsRv32fInstructionHandler<Handler>
    struct Rv32imfDispatcher : public Handler
    {
        using Item = Handler::Item;

        // Decodes the input word to an RV32IMF instruction and dispatches it to a handler.
        // clang-format off
        auto Dispatch(u32 code) -> Item
        {
            Handler& self = static_cast<Handler&>(*this);
            Instruction c(code);

            switch (code) {
                case 0x00000073: return self.Ecall();
                case 0x00100073: return self.Ebreak();
            }
            switch (code & 0xfff0707f) {
                case 0xe0000053: return self.Fmv_x_w(c.Rd(), c.Rs1());
                case 0xe0001053: return self.Fclass_s(c.Rd(), c.Rs1());
                case 0xf0000053: return self.Fmv_w_x(c.Rd(), c.Rs1());
            }
            switch (code & 0xfff0007f) {
                case 0x58000053: return self.Fsqrt_s(c.Rd(), c.Rs1(), c.Rm());
                case 0xc0000053: return self.Fcvt_w_s(c.Rd(), c.Rs1(), c.Rm());
                case 0xc0100053: return self.Fcvt_wu_s(c.Rd(), c.Rs1(), c.Rm());
                case 0xd0000053: return self.Fcvt_s_w(c.Rd(), c.Rs1(), c.Rm());
                case 0xd0100053: return self.Fcvt_s_wu(c.Rd(), c.Rs1(), c.Rm());
            }
            switch (code & 0xfe00707f) {
                case 0x00000033: return self.Add(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40000033: return self.Sub(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001033: return self.Sll(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00002033: return self.Slt(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00003033: return self.Sltu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00004033: return self.Xor(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00005033: return self.Srl(c.Rd(), c.Rs1(), c.Rs2());
                case 0x40005033: return self.Sra(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00006033: return self.Or(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00007033: return self.And(c.Rd(), c.Rs1(), c.Rs2());
                case 0x00001013: return self.Slli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x00005013: return self.Srli(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x40005013: return self.Srai(c.Rd(), c.Rs1(), c.Shamtw());
                case 0x02000033: return self.Mul(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02001033: return self.Mulh(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02002033: return self.Mulhsu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02003033: return self.Mulhu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02004033: return self.Div(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02005033: return self.Divu(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02006033: return self.Rem(c.Rd(), c.Rs1(), c.Rs2());
                case 0x02007033: return self.Remu(c.Rd(), c.Rs1(), c.Rs2());
            }
            switch (code & 0xfe00707f) {
                case 0x20000053: return self.Fsgnj_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0x20001053: return self.Fsgnjn_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0x20002053: return self.Fsgnjx_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0x28000053: return self.Fmin_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0x28001053: return self.Fmax_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0xa0000053: return self.Fle_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0xa0001053: return self.Flt_s(c.Rd(), c.Rs1(), c.Rs2());
                case 0xa0002053: return self.Feq_s(c.Rd(), c.Rs1(), c.Rs2());
            }
            switch (code & 0xfe00007f) {
                case 0x00000053: return self.Fadd_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rm());
                case 0x08000053: return self.Fsub_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rm());
                case 0x10000053: return self.Fmul_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rm());
                case 0x18000053: return self.Fdiv_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rm());
            }
            switch (code & 0x0000707f) {
                case 0x00000063: return self.Beq(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00001063: return self.Bne(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00004063: return self.Blt(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00005063: return self.Bge(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00006063: return self.Bltu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00007063: return self.Bgeu(c.Rs1(), c.Rs2(), c.Bimmediate());
                case 0x00000067: return self.Jalr(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000013: return self.Addi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002013: return self.Slti(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00003013: return self.Sltiu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004013: return self.Xori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00006013: return self.Ori(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00007013: return self.Andi(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000003: return self.Lb(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00001003: return self.Lh(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002003: return self.Lw(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00004003: return self.Lbu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00005003: return self.Lhu(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00000023: return self.Sb(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00001023: return self.Sh(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x00002023: return self.Sw(c.Rs1(), c.Rs2(), c.Simmediate());
                case 0x0000000f: return self.Fence(c.Fm(), c.Rd(), c.Rs1());
                case 0x00002007: return self.Flw(c.Rd(), c.Rs1(), c.Iimmediate());
                case 0x00002027: return self.Fsw(c.Rs1(), c.Rs2(), c.Simmediate());
            }
            switch (code & 0x0600007f) {
                case 0x00000043: return self.Fmadd_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rs3(), c.Rm());
                case 0x00000047: return self.Fmsub_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rs3(), c.Rm());
                case 0x0000004b: return self.Fnmsub_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rs3(), c.Rm());
                case 0x0000004f: return self.Fnmadd_s(c.Rd(), c.Rs1(), c.Rs2(), c.Rs3(), c.Rm());
            }
            switch (code & 0x0000007f) {
                case 0x0000006f: return self.Jal(c.Rd(), c.Jimmediate());
                case 0x00000037: return self.Lui(c.Rd(), c.Uimmediate());
                case 0x00000017: return self.Auipc(c.Rd(), c.Uimmediate());
            }
            return self.Illegal(code);
        }
        // clang-format on
    };

    // End of auto-generated code.

} // namespace arviss
