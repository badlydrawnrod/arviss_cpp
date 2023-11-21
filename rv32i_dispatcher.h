#pragma once

#include "core_concepts.h"
#include "instruction.h"
#include "rv32i_concepts.h"

// TYPES: dispatcher for handler that satisfies IsRv32iInstructionHandler. RV32i specific.
// Satifies: TODO: which concepts?

// This code was generated by `.\make_dispatcher.py c++`. Do not edit.

// A dispatcher for RV32I instructions.
template<typename Handler>
    requires IsRv32iInstructionHandler<Handler>
struct MRv32iDispatcher : public Handler
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
