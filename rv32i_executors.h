#pragma once

#include "core_concepts.h"
#include "rv32i_concepts.h"

// An Rv32i instruction handler that executes instructions on an integer core.
template<IsIntegerCore T>
class MRv32iIntegerCoreExecutor : public T
{
    auto Self() -> T& { return static_cast<T&>(*this); }

    // Sign extend a byte.
    static auto SExt(u8 byte) -> i32 { return static_cast<i32>(static_cast<i16>(static_cast<i8>(byte))); }

    // Sign extend a halfword.
    static auto SExt(u16 halfWord) -> i32 { return static_cast<i32>(static_cast<i16>(halfWord)); }

public:
    using Item = void;

    // Illegal instruction.

    auto Illegal(u32 ins) -> Item
    {
        auto& self = Self();
        self.RaiseTrap(TrapType::IllegalInstruction, ins);
    }

    // B-type instructions.

    auto Beq(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // pc <- pc + ((rs1 == rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) == self.Rx(rs2))
        {
            self.SetNextPc(self.Pc() + bimm); // TODO: wrapping add
        }
    }

    auto Bne(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // pc <- pc + ((rs1 != rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) != self.Rx(rs2))
        {
            self.SetNextPc(self.Pc() + bimm); // TODO: wrapping add
        }
    }

    auto Blt(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Signed.
        // pc <- pc + ((rs1 < rs2) ? imm_b : 4)
        auto& self = Self();
        if (static_cast<i32>(self.Rx(rs1)) < static_cast<i32>(self.Rx(rs2)))
        {
            self.SetNextPc(self.Pc() + bimm); // TODO: wrapping add
        }
    }

    auto Bge(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Signed.
        // pc <- pc + ((rs1 >= rs2) ? imm_b : 4)
        auto& self = Self();
        if (static_cast<i32>(self.Rx(rs1)) >= static_cast<i32>(self.Rx(rs2)))
        {
            self.SetNextPc(self.Pc() + bimm); // TODO: wrapping add
        }
    }

    auto Bltu(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Unsigned.
        // pc <- pc + ((rs1 < rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) < self.Rx(rs2))
        {
            self.SetNextPc(self.Pc() + bimm); // TODO: wrapping add
        }
    }

    auto Bgeu(Reg rs1, Reg rs2, u32 bimm) -> Item
    {
        // Unsigned.
        // pc <- pc + ((rs1 >= rs2) ? imm_b : 4)
        auto& self = Self();
        if (self.Rx(rs1) >= self.Rx(rs2))
        {
            self.SetNextPc(self.Pc() + bimm); // TODO: wrapping add
        }
    }

    // I-type instructions.

    auto Lb(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- sx(m8(rs1 + imm_i)), pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + iimm;
        const auto byte = self.Read8(address);
        self.Wx(rd, static_cast<u32>(SExt(byte)));
    }

    auto Lh(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- sx(m16(rs1 + imm_i)), pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + iimm;
        const auto halfWord = self.Read16(address);
        self.Wx(rd, static_cast<u32>(SExt(halfWord)));
    }

    auto Lw(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- sx(m32(rs1 + imm_i)), pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + iimm;
        const auto word = self.Read32(address);
        self.Wx(rd, word);
    }

    auto Lbu(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- zx(m8(rs1 + imm_i)), pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + iimm;
        const auto byte = self.Read8(address);
        self.Wx(rd, static_cast<u32>(byte));
    }

    auto Lhu(Reg rd, Reg rs1, u32 iimm) -> Item
    {
        // rd <- zx(m16(rs1 + imm_i)), pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + iimm;
        const auto halfWord = self.Read16(address);
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
        auto rs1Before = self.Rx(rs1); // Because rd and rs1 might be the same register.
        self.Wx(rd, self.Pc() + 4);    // TODO: wrapping add
        self.SetNextPc(rs1Before + (iimm) & ~1);
    }

    // S-type instructions.

    auto Sb(Reg rs1, Reg rs2, u32 simm) -> Item
    {
        // m8(rs1 + imm_s) <- rs2[7:0], pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + simm;
        self.Write8(address, self.Rx(rs2) & 0xff);
    }

    auto Sh(Reg rs1, Reg rs2, u32 simm) -> Item
    {
        // m16(rs1 + imm_s) <- rs2[15:0], pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + simm;
        self.Write16(address, self.Rx(rs2) & 0xffff);
    }

    auto Sw(Reg rs1, Reg rs2, u32 simm) -> Item
    {
        // m32(rs1 + imm_s) <- rs2[31:0], pc += 4
        auto& self = Self();
        const auto address = self.Rx(rs1) + simm;
        self.Write32(address, self.Rx(rs2));
    }

    // U-type instructions.

    auto Auipc(Reg rd, u32 uimm) -> Item
    {
        // rd <- pc + imm_u, pc += 4
        auto& self = Self();
        self.Wx(rd, self.Pc() + uimm); // TODO: wrapping add
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
        self.Wx(rd, self.Pc() + 4);       // TODO: wrapping add
        self.SetNextPc(self.Pc() + jimm); // TODO: wrapping add
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

    auto Ecall() -> Item
    {
        auto& self = Self();
        self.RaiseTrap(TrapType::EnvironmentCallFromMMode);
    }

    auto Ebreak() -> Item
    {
        auto& self = Self();
        self.RaiseTrap(TrapType::Breakpoint);
    }
};

template<IsIntegerCore T>
class MRv32imIntegerCoreExecutor : public MRv32iIntegerCoreExecutor<T>
{
    auto Self() -> T& { return static_cast<T&>(*this); }

public:
    using Item = MRv32iIntegerCoreExecutor<T>::Item;

    // TODO: Implement the 'M' extension.

    auto Mul(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Mulh(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Mulhsu(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Mulhu(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Div(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Divu(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Rem(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }

    auto Remu(Reg rd, Reg rs1, Reg rs2) -> Item { auto& self = Self(); }
};

template<IsIntegerCore T>
class MRv32icIntegerCoreExecutor : public MRv32iIntegerCoreExecutor<T>
{
    auto Self() -> T& { return static_cast<T&>(*this); }

public:
    using Item = MRv32iIntegerCoreExecutor<T>::Item;

    auto C_ebreak() -> Item { this->Ebreak(); }

    auto C_jr(Reg rs1n0) -> Item
    {
        // jalr x0, 0(rs1)
        auto& self = Self();
        self.SetNextPc(self.Rx(rs1n0) & ~1);
    }

    auto C_jalr(Reg rs1n0) -> Item
    {
        // jalr x1, 0(rs1)
        auto& self = Self();
        auto rs1Before = self.Rx(rs1n0);      // Because rs1 might be RA.
        self.Wx(RegNames::RA, self.Pc() + 2); // TODO: wrapping add.
        self.SetNextPc(rs1Before & ~1);
    }

    auto C_nop(u32 u) -> Item
    {
        // nop
    }

    auto C_addi16sp(u32 imm) -> Item
    {
        // addi x2, x2, nzimm[9:4]
        this->Addi(RegNames::SP, RegNames::SP, imm);
    }

    auto C_sub(Reg rdrs1p, Reg rs2p) -> Item
    {
        // sub rdp, rdp, rs2p
        this->Sub(rdrs1p, rdrs1p, rs2p);
    }

    auto C_xor(Reg rdrs1p, Reg rs2p) -> Item
    {
        // xor rdp, rdp, rs2p
        this->Xor(rdrs1p, rdrs1p, rs2p);
    }

    auto C_or(Reg rdrs1p, Reg rs2p) -> Item
    {
        // or rdp, rdp, rs2p
        this->Or(rdrs1p, rdrs1p, rs2p);
    }

    auto C_and(Reg rdrs1p, Reg rs2p) -> Item
    {
        // and rdp, rdp, rs2p
        this->And(rdrs1p, rdrs1p, rs2p);
    }

    auto C_andi(Reg rsrs1p, u32 imm) -> Item
    {
        // andi rdp, rdp, imm[5:0]
        this->Andi(rsrs1p, rsrs1p, imm);
    }

    auto C_srli(Reg rdrs1p, u32 imm) -> Item
    {
        // srli rdp, rdp, shamt[5:0]
        this->Srli(rdrs1p, rdrs1p, imm);
    }

    auto C_srai(Reg rdrs1p, u32 imm) -> Item
    {
        // srai rdp, rdp, shamt[5:0]
        this->Srai(rdrs1p, rdrs1p, imm);
    }

    auto C_mv(Reg rd, Reg rs2n0) -> Item
    {
        // add rd, x0, rs2
        this->Add(rd, RegNames::ZERO, rs2n0);
    }

    auto C_add(Reg rdrs1, Reg rs2n0) -> Item
    {
        // add rd, rd, rs2
        this->Add(rdrs1, rdrs1, rs2n0);
    }

    auto C_addi4spn(Reg rdp, u32 imm) -> Item
    {
        // addi rdp, x2, nzuimm[9:2]
        this->Addi(rdp, RegNames::SP, imm);
    }

    auto C_lw(Reg rdp, Reg rs1p, u32 imm) -> Item
    {
        // lw rdp, offset[6:2](rs1p)
        this->Lw(rdp, rs1p, imm);
    }

    auto C_sw(Reg rs1p, Reg rs2p, u32 imm) -> Item
    {
        // sw rs2p, offset[6:2](rs1p)
        this->Sw(rs1p, rs2p, imm);
    }

    auto C_addi(Reg rdrs1n0, u32 imm) -> Item
    {
        // addi rd, rd, nzimm[5:0]
        this->Addi(rdrs1n0, rdrs1n0, imm);
    }

    auto C_li(Reg rd, u32 imm) -> Item
    {
        // addi rd, x0, imm[5:0]
        this->Addi(rd, RegNames::ZERO, imm);
    }

    auto C_lui(Reg rdn2, u32 imm) -> Item
    {
        // lui rd, nzimm[17:12]
        this->Lui(rdn2, imm);
    }

    auto C_j(u32 imm) -> Item
    {
        // jal x0, offset[11:1]
        auto& self = Self();
        self.SetNextPc(self.Pc() + imm); // TODO: wrapping add
    }

    auto C_beqz(Reg rs1p, u32 imm) -> Item
    {
        // beq rs1p, x0, offset[8:1]
        this->Beq(rs1p, RegNames::ZERO, imm);
    }

    auto C_bnez(Reg rs1p, u32 imm) -> Item
    {
        // bne rs1p, x0, offset[8:1]
        this->Bne(rs1p, RegNames::ZERO, imm);
    }

    auto C_lwsp(Reg rdn0, u32 imm) -> Item
    {
        // lw rd, offset[7:2](x2)
        this->Lw(rdn0, RegNames::SP, imm);
    }

    auto C_swsp(Reg rs2, u32 imm) -> Item
    {
        // sw rs2, offset[7:2](x2)
        this->Sw(RegNames::SP, rs2, imm);
    }

    auto C_jal(u32 imm) -> Item
    {
        // jal x1, offset[11:1]
        auto& self = Self();
        self.Wx(RegNames::RA, self.Pc() + 2); // TODO: wrapping add
        self.SetNextPc(self.Pc() + imm);      // TODO: wrapping add
    }

    auto C_slli(Reg rdrs1n0, u32 imm) -> Item
    {
        // slli rd, rd, shamt[5:0]
        this->Slli(rdrs1n0, rdrs1n0, imm);
    }
};
