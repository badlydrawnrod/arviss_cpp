#pragma once

#include "dispatcher.h"
#include "types.h"

#include <format>
#include <string>

auto Abi(Reg r) -> const char*
{
    static const char* abiNames[] = {
            "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
            "a6",   "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
    };
    return abiNames[r];
}

// A mixin instruction handler for a disassembler for Rv32i instructions.
struct MRv32iDisassembler
{
    using Item = std::string;

    auto Illegal(u32 ins) -> Item { return std::format("illegal instruction: {:04x}", ins); }
    auto Beq(Reg rs1, Reg rs2, u32 bimm) -> Item { return std::format("beq\t{}, {}, {}", Abi(rs1), Abi(rs2), static_cast<i32>(bimm)); }
    auto Bne(Reg rs1, Reg rs2, u32 bimm) -> Item { return std::format("bne\t{}, {}, {}", Abi(rs1), Abi(rs2), static_cast<i32>(bimm)); }
    auto Blt(Reg rs1, Reg rs2, u32 bimm) -> Item { return std::format("blt\t{}, {}, {}", Abi(rs1), Abi(rs2), static_cast<i32>(bimm)); }
    auto Bge(Reg rs1, Reg rs2, u32 bimm) -> Item { return std::format("bge\t{}, {}, {}", Abi(rs1), Abi(rs2), static_cast<i32>(bimm)); }
    auto Bltu(Reg rs1, Reg rs2, u32 bimm) -> Item { return std::format("bltu\t{}, {}, {}", Abi(rs1), Abi(rs2), static_cast<i32>(bimm)); }
    auto Bgeu(Reg rs1, Reg rs2, u32 bimm) -> Item { return std::format("bgeu\t{}, {}, {}", Abi(rs1), Abi(rs2), static_cast<i32>(bimm)); }
    auto Lb(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("lb\t{}, {}({})", Abi(rd), static_cast<i32>(iimm), Abi(rs1)); }
    auto Lh(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("lh\t{}, {}({})", Abi(rd), static_cast<i32>(iimm), Abi(rs1)); }
    auto Lw(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("lw\t{}, {}({})", Abi(rd), static_cast<i32>(iimm), Abi(rs1)); }
    auto Lbu(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("lbu\t{}, {}({})", Abi(rd), static_cast<i32>(iimm), Abi(rs1)); }
    auto Lhu(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("lhu\t{}, {}({})", Abi(rd), static_cast<i32>(iimm), Abi(rs1)); }
    auto Addi(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("addi\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Slti(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("slti\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Sltiu(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("sltiu\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Xori(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("xori\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Ori(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("ori\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Andi(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("andi\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Jalr(Reg rd, Reg rs1, u32 iimm) -> Item { return std::format("jalr\t{}, {}, {}", Abi(rd), Abi(rs1), static_cast<i32>(iimm)); }
    auto Sb(Reg rs1, Reg rs2, u32 simm) -> Item { return std::format("sb\t{}, {}({})", Abi(rs2), static_cast<i32>(simm), Abi(rs1)); }
    auto Sh(Reg rs1, Reg rs2, u32 simm) -> Item { return std::format("sh\t{}, {}({})", Abi(rs2), static_cast<i32>(simm), Abi(rs1)); }
    auto Sw(Reg rs1, Reg rs2, u32 simm) -> Item { return std::format("sw\t{}, {}({})", Abi(rs2), static_cast<i32>(simm), Abi(rs1)); }
    auto Auipc(Reg rd, u32 uimm) -> Item { return std::format("auipc\t{}, {}", Abi(rd), static_cast<i32>(uimm) >> 12); }
    auto Lui(Reg rd, u32 uimm) -> Item { return std::format("lui\t{}, {}", Abi(rd), static_cast<i32>(uimm) >> 12); }
    auto Jal(Reg rd, u32 jimm) -> Item { return std::format("jal\t{}, {}", Abi(rd), static_cast<i32>(jimm)); }
    auto Add(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("add\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Sub(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("sub\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Sll(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("sll\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Slt(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("slt\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Sltu(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("sltu\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Xor(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("xor\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Srl(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("srl\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Sra(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("sra\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Or(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("or\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto And(Reg rd, Reg rs1, Reg rs2) -> Item { return std::format("and\t{}, {}, {}", Abi(rd), Abi(rs1), Abi(rs2)); }
    auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item { return std::format("slli\t{}, {}, {}", Abi(rd), Abi(rs1), shamt); }
    auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item { return std::format("srli\t{}, {}, {}", Abi(rd), Abi(rs1), shamt); }
    auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item { return std::format("srai\t{}, {}, {}", Abi(rd), Abi(rs1), shamt); }
    auto Fence([[maybe_unused]] u32 fm, [[maybe_unused]] Reg rd, [[maybe_unused]] Reg rs1) -> Item { return "fence"; }
    auto Ecall() -> Item { return "ecall"; }
    auto Ebreak() -> Item { return "ebreak"; }
};

// A disassembler.
using Disassembler = MRv32iDispatcher<MRv32iDisassembler>;
