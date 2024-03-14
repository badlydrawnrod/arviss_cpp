#pragma once

#include "arviss/arviss.h"
#include "arviss/rv32/dispatchers.h"

#include <format>
#include <string>

namespace arviss
{
    auto Abi(Reg r) -> const char*
    {
        static const char* abiNames[] = {
                "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2", "s0", "s1", "a0",  "a1",  "a2", "a3", "a4", "a5",
                "a6",   "a7", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6",
        };
        return abiNames[r];
    }

    // An instruction handler for a disassembler for Rv32i instructions.
    struct Rv32iDisassemblingHandler
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

    // A disassembler for Rv32i instructions.
    using Rv32iDisassembler = Rv32iDispatcher<Rv32iDisassemblingHandler>;

    // An instruction handler for a disassembler for Rv32ic instructions. This implementation cheats slightly by delegating to the handler for non-compressed
    // instructions. If you want to see compressed instructions then rewrite it in terms of formatted strings like the parent handler.
    struct Rv32icDisassemblingHandler : public Rv32iDisassemblingHandler
    {
        using Item = Rv32iDisassemblingHandler::Item;

        auto C_ebreak() -> Item { return Ebreak(); }                                       // ebreak
        auto C_jr(Reg rs1n0) -> Item { return Jalr(RegNames::ZERO, rs1n0, 0); }            // jalr x0, 0(rs1)
        auto C_jalr(Reg rs1n0) -> Item { return Jalr(RegNames::RA, rs1n0, 0); }            // jalr x1, 0(rs1)
        auto C_nop(u32 u) -> Item { return "c_nop"; }                                      // nop
        auto C_addi16sp(u32 imm) -> Item { return Addi(RegNames::SP, RegNames::SP, imm); } // addi x2, x2, nzimm[9:4]
        auto C_sub(Reg rdrs1p, Reg rs2p) -> Item { return Sub(rdrs1p, rdrs1p, rs2p); }     // sub rdp, rdp, rs2p
        auto C_xor(Reg rdrs1p, Reg rs2p) -> Item { return Xor(rdrs1p, rdrs1p, rs2p); }     // xor rdp, rdp, rs2p
        auto C_or(Reg rdrs1p, Reg rs2p) -> Item { return Or(rdrs1p, rdrs1p, rs2p); }       // or rdp, rdp, rs2p
        auto C_and(Reg rdrs1p, Reg rs2p) -> Item { return And(rdrs1p, rdrs1p, rs2p); }     // and rdp, rdp, rs2p
        auto C_andi(Reg rsrs1p, u32 imm) -> Item { return Andi(rsrs1p, rsrs1p, imm); }     // andi rdp, rdp, imm[5:0]
        auto C_srli(Reg rdrs1p, u32 imm) -> Item { return Srli(rdrs1p, rdrs1p, imm); }     // srli rdp, rdp, shamt[5:0]
        auto C_srai(Reg rdrs1p, u32 imm) -> Item { return Srai(rdrs1p, rdrs1p, imm); }     // srai rdp, rdp, shamt[5:0]
        auto C_mv(Reg rd, Reg rs2n0) -> Item { return Add(rd, RegNames::ZERO, rs2n0); }    // add rd, x0, rs2
        auto C_add(Reg rdrs1, Reg rs2n0) -> Item { return Add(rdrs1, rdrs1, rs2n0); }      // add rd, rd, rs2
        auto C_addi4spn(Reg rdp, u32 imm) -> Item { return Addi(rdp, RegNames::SP, imm); } // addi rdp, x2, nzuimm[9:2]
        auto C_lw(Reg rdp, Reg rs1p, u32 imm) -> Item { return Lw(rdp, rs1p, imm); }       // lw rdp, offset[6:2](rs1p)
        auto C_sw(Reg rs1p, Reg rs2p, u32 imm) -> Item { return Sw(rs1p, rs2p, imm); }     // sw rs2p, offset[6:2](rs1p)
        auto C_addi(Reg rdrs1n0, u32 imm) -> Item { return Addi(rdrs1n0, rdrs1n0, imm); }  // addi rd, rd, nzimm[5:0]
        auto C_li(Reg rd, u32 imm) -> Item { return Addi(rd, RegNames::ZERO, imm); }       // addi rd, x0, imm[5:0]
        auto C_lui(Reg rdn2, u32 imm) -> Item { return Lui(rdn2, imm); }                   // lui rd, nzimm[17:12]
        auto C_j(u32 imm) -> Item { return Jal(RegNames::ZERO, imm); }                     // jal x0, offset[11:1]
        auto C_beqz(Reg rs1p, u32 imm) -> Item { return Beq(rs1p, RegNames::ZERO, imm); }  // beq rs1p, x0, offset[8:1]
        auto C_bnez(Reg rs1p, u32 imm) -> Item { return Bne(rs1p, RegNames::ZERO, imm); }  // bne rs1p, x0, offset[8:1]
        auto C_lwsp(Reg rdn0, u32 imm) -> Item { return Lw(rdn0, RegNames::SP, imm); }     // lw rd, offset[7:2](x2)
        auto C_swsp(Reg rs2, u32 imm) -> Item { return Sw(RegNames::SP, rs2, imm); }       // sw rs2, offset[7:2](x2)
        auto C_jal(u32 imm) -> Item { return Jal(RegNames::RA, imm); }                     // jal x1, offset[11:1]
        auto C_slli(Reg rdrs1n0, u32 imm) -> Item { return Slli(rdrs1n0, rdrs1n0, imm); }  // slli rd, rd, shamt[5:0]
    };

    // A disassembler for Rv32ic instructions.
    using Rv32icDisassembler = Rv32icDispatcher<Rv32icDisassemblingHandler>;
} // namespace arviss
