#pragma once

#include "core_concepts.h"

#include <format>

// A mixin implementation of an instruction handler that operates on an integer CPU.
template<IsIntCpu T>
class MRv32iHandler : public T
{
    auto Self() -> T& { return static_cast<T&>(*this); }

    // Sign extend a byte.
    static auto SExt(u8 byte) -> i32 { return static_cast<i32>(static_cast<i16>(static_cast<i8>(byte))); }

    // Sign extend a halfword.
    static auto SExt(u16 halfWord) -> i32 { return static_cast<i32>(static_cast<i16>(halfWord)); }

public:
    using Item = void;

    // Illegal instruction.

    auto Illegal(u32 ins) -> Item { throw std::runtime_error(std::format("Illegal instruction: {:04x}", ins)); }

    // B-type instructions.

    auto Beq(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // pc <- pc + ((rs1 == rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) == self.Rx(rs2)) { self.SetNextPc(self.Pc() + (bimm)); }
    }

    auto Bne(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // pc <- pc + ((rs1 != rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) != self.Rx(rs2)) { self.SetNextPc(self.Pc() + (bimm)); }
    }

    auto Blt(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Signed.
        // pc <- pc + ((rs1 < rs2) ? imm_b : 4)
        auto& self = Self();
        if (static_cast<i32>(self.Rx(rs1)) < static_cast<i32>(self.Rx(rs2))) { self.SetNextPc(self.Pc() + (bimm)); }
    }

    auto Bge(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Signed.
        // pc <- pc + ((rs1 >= rs2) ? imm_b : 4)
        auto& self = Self();
        if (static_cast<i32>(self.Rx(rs1)) >= static_cast<i32>(self.Rx(rs2))) { self.SetNextPc(self.Pc() + (bimm)); }
    }

    auto Bltu(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Unsigned.
        // pc <- pc + ((rs1 < rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) < self.Rx(rs2)) { self.SetNextPc(self.Pc() + (bimm)); }
    }

    auto Bgeu(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Unsigned.
        // pc <- pc + ((rs1 >= rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) >= self.Rx(rs2)) { self.SetNextPc(self.Pc() + (bimm)); }
    }

    // I-type instructions.

    auto Lb(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- sx(m8(rs1 + imm_i)), pc += 4
        auto& self = Self();
        auto byte = self.Read8(self.Rx(rs1) + (iimm));
        self.Wx(rd, static_cast<u32>(SExt(byte)));
    }

    auto Lh(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- sx(m16(rs1 + imm_i)), pc += 4
        auto& self = Self();
        auto halfWord = self.Read16(self.Rx(rs1) + (iimm));
        self.Wx(rd, static_cast<u32>(SExt(halfWord)));
    }

    auto Lw(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- sx(m32(rs1 + imm_i)), pc += 4
        auto& self = Self();
        auto word = self.Read32(self.Rx(rs1) + (iimm));
        self.Wx(rd, word);
    }

    auto Lbu(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- zx(m8(rs1 + imm_i)), pc += 4
        auto& self = Self();
        auto byte = self.Read8(self.Rx(rs1) + (iimm));
        self.Wx(rd, static_cast<u32>(byte));
    }

    auto Lhu(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- zx(m16(rs1 + imm_i)), pc += 4
        auto& self = Self();
        auto halfWord = self.Read16(self.Rx(rs1) + (iimm));
        self.Wx(rd, static_cast<u32>(halfWord));
    }

    auto Addi(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- rs1 + imm_i, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) + (iimm));
    }

    auto Slti(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // Signed.
        // rd <- (rs1 < imm_i) ? 1 : 0, pc += 4
        auto& self = Self();
        auto xreg_rs1 = static_cast<i32>(self.Rx(rs1));
        self.Wx(rd, xreg_rs1 < static_cast<i32>(iimm) ? 1 : 0);
    }

    auto Sltiu(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // Unsigned.
        // rd <- (rs1 < imm_i) ? 1 : 0, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) < iimm ? 1 : 0);
    }

    auto Xori(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- rs1 ^ imm_i, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) ^ iimm);
    }

    auto Ori(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- rs1 | imm_i, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) | iimm);
    }

    auto Andi(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- rs1 & imm_i, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) & iimm);
    }

    auto Jalr(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- pc + 4, pc <- (rs1 + imm_i) & ~1
        auto& self = Self();
        auto rs1_before = self.Rx(rs1); // Because rd and rs1 might be the same register.
        self.Wx(rd, self.Pc() + (4));
        self.SetNextPc(rs1_before + (iimm) & ~1);
    }

    // S-type instructions.

    auto Sb(Reg rs1, Reg rs2, u32 simm) -> Item
    {
        // m8(rs1 + imm_s) <- rs2[7:0], pc += 4
        auto& self = Self();
        self.Write8(self.Rx(rs1) + (simm), self.Rx(rs2) & 0xff);
    }

    auto Sh(Reg rs1, Reg rs2, u32 simm) -> Item
    {
        // m16(rs1 + imm_s) <- rs2[15:0], pc += 4
        auto& self = Self();
        self.Write16(self.Rx(rs1) + (simm), self.Rx(rs2) & 0xffff);
    }

    auto Sw(Reg rs1, Reg rs2, u32 simm) -> Item
    {
        // m32(rs1 + imm_s) <- rs2[31:0], pc += 4
        auto& self = Self();
        self.Write32(self.Rx(rs1) + (simm), self.Rx(rs2));
    }

    // U-type instructions.

    auto Auipc(Reg rd, u32 uimm) -> Item
    {
        // rd <- pc + imm_u, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Pc() + (uimm));
    }

    auto Lui(Reg rd, u32 uimm) -> Item
    {
        // rd <- imm_u, pc += 4
        auto& self = Self();
        self.Wx(rd, uimm);
    }

    // J-type instructions.

    auto Jal(Reg rd, u32 jimm) -> Item
    {
        // rd <- pc + 4, pc <- pc + imm_j
        auto& self = Self();
        self.Wx(rd, self.Pc() + (4));
        self.SetNextPc(self.Pc() + (jimm));
    }

    // Arithmetic instructions.

    auto Add(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 + rs2, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) + (self.Rx(rs2)));
    }

    auto Sub(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 - rs2, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) - (self.Rx(rs2)));
    }

    auto Sll(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 << (rs2 % XLEN), pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) << (self.Rx(rs2) % 32));
    }

    auto Slt(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // Signed.
        // rd <- (rs1 < rs2) ? 1 : 0, pc += 4
        auto& self = Self();
        auto xreg_rs1 = static_cast<i32>(self.Rx(rs1));
        auto xreg_rs2 = static_cast<i32>(self.Rx(rs2));
        self.Wx(rd, xreg_rs1 < xreg_rs2 ? 1 : 0);
    }

    auto Sltu(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- (rs1 < rs2) ? 1 : 0, pc += 4
        auto& self = Self();
        auto xreg_rs1 = self.Rx(rs1);
        auto xreg_rs2 = self.Rx(rs2);
        self.Wx(rd, xreg_rs1 < xreg_rs2 ? 1 : 0);
    }

    auto Xor(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 ^ rs2, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) ^ self.Rx(rs2));
    }

    auto Srl(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 >> (rs2 % XLEN), pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) >> (self.Rx(rs2) % 32));
    }

    auto Sra(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 >> (rs2 % XLEN), pc += 4
        auto& self = Self();
        auto xreg_rs1 = static_cast<i32>(self.Rx(rs1));
        auto shift = static_cast<i32>((self.Rx(rs2) % 32));
        self.Wx(rd, static_cast<u32>(xreg_rs1 >> shift));
    }

    auto Or(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 | rs2, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) | self.Rx(rs2));
    }

    auto And(Reg rd, Reg rs1, Reg rs2) -> Item
    {
        // rd <- rs1 & rs2, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) & self.Rx(rs2));
    }

    // Immediate shift instructions.

    auto Slli(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) << shamt);
    }

    auto Srli(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        auto& self = Self();
        self.Wx(rd, self.Rx(rs1) >> shamt);
    }

    auto Srai(Reg rd, Reg rs1, u32 shamt) -> Item
    {
        auto& self = Self();
        auto xreg_rs = static_cast<i32>(self.Rx(rs1));
        self.Wx(rd, static_cast<u32>(xreg_rs >> static_cast<i32>(shamt)));
    }

    auto Fence([[maybe_unused]] u32 fm, [[maybe_unused]] Reg rd, [[maybe_unused]] Reg rs1) -> Item
    {
        // Do nothing.
    }

    auto Ecall() -> Item { throw std::runtime_error("ecall not implemented yet"); }

    auto Ebreak() -> Item { throw std::runtime_error("ebreak (breakpoint)"); }
};
