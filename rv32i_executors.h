#pragma once

#include "core_concepts.h"
#include "rv32i_concepts.h"

#include <bit>
#include <cmath>
#include <limits>

namespace arviss
{
    // An Rv32i instruction handler that executes instructions on an integer core. BYO core.
    template<IsIntegerCore T>
    class Rv32iIntegerCoreExecutor : public T
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

    // An Rv32im instruction handler that executes instructions on an integer core. BYO core.
    template<IsIntegerCore T>
    class Rv32imIntegerCoreExecutor : public Rv32iIntegerCoreExecutor<T>
    {
        auto Self() -> T& { return static_cast<T&>(*this); }

    public:
        using Item = Rv32iIntegerCoreExecutor<T>::Item;

        // TODO: Implement the 'M' extension.

        auto Mul(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- rs1 * rs2, pc += 4
            auto& self = Self();
            self.Wx(rd, self.Rx(rs1) * self.Rx(rs2));
        }

        auto Mulh(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto xreg_rs1 = static_cast<i64>(static_cast<i32>(self.Rx(rs1)));
            auto xreg_rs2 = static_cast<i64>(static_cast<i32>(self.Rx(rs2)));
            auto t = (xreg_rs1 * xreg_rs2) >> 32;
            self.Wx(rd, static_cast<u32>(t));
        }

        auto Mulhsu(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto xreg_rs1 = static_cast<i64>(static_cast<i32>(self.Rx(rs1)));
            auto xreg_rs2 = static_cast<i64>(static_cast<u64>(self.Rx(rs2)));
            auto t = (xreg_rs1 * xreg_rs2) >> 32;
            self.Wx(rd, static_cast<u32>(t));
        }

        auto Mulhu(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto xreg_rs1 = static_cast<u64>(self.Rx(rs1));
            auto xreg_rs2 = static_cast<u64>(self.Rx(rs2));
            auto t = (xreg_rs1 * xreg_rs2) >> 32;
            self.Wx(rd, static_cast<u32>(t));
        }

        auto Div(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto dividend = static_cast<i32>(self.Rx(rs1));
            auto divisor = static_cast<i32>(self.Rx(rs2));
            // Check for signed division overflow.
            if ((static_cast<u32>(dividend) != 0x80000000) || divisor != -1)
            {
                self.Wx(rd, divisor != 0 ? dividend / divisor : std::numeric_limits<u32>::max());
            }
            else
            {
                // Signed division overflow occurred.
                self.Wx(rd, static_cast<u32>(dividend));
            }
        }

        auto Divu(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto dividend = self.Rx(rs1);
            auto divisor = self.Rx(rs2);
            self.Wx(rd, divisor != 0 ? dividend / divisor : std::numeric_limits<u32>::max());
        }

        auto Rem(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto dividend = static_cast<i32>(self.Rx(rs1));
            auto divisor = static_cast<i32>(self.Rx(rs2));
            // Check for signed division overflow.
            if ((static_cast<u32>(dividend) != 0x80000000) || divisor != -1)
            {
                self.Wx(rd, divisor != 0 ? static_cast<u32>(dividend % divisor) : static_cast<u32>(dividend));
            }
            else
            {
                // Signed division overflow occurred.
                self.Wx(rd, 0);
            }
        }

        auto Remu(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            auto& self = Self();
            auto dividend = self.Rx(rs1);
            auto divisor = self.Rx(rs2);
            self.Wx(rd, divisor != 0 ? dividend % divisor : dividend);
        }
    };

    // An Rv32ic instruction handler that executes instructions on an integer core. BYO core.
    template<IsIntegerCore T>
    class Rv32icIntegerCoreExecutor : public Rv32iIntegerCoreExecutor<T>
    {
        auto Self() -> T& { return static_cast<T&>(*this); }

    public:
        using Item = Rv32iIntegerCoreExecutor<T>::Item;

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

    // An Rv32imf instruction handler that executes instructions on a floating point core. BYO core.
    template<IsFloatCore T>
    class Rv32imfFloatCoreExecutor : public Rv32imIntegerCoreExecutor<T>
    {
        auto Self() -> T& { return static_cast<T&>(*this); }

    public:
        using Item = Rv32imIntegerCoreExecutor<T>::Item;

        auto Fmv_x_w(Reg rd, Reg rs1) -> Item
        {
            // bits(rd) <- bits(rs1)
            auto& self = Self();
            self.Wx(rd, std::bit_cast<u32>(self.Rf(rs1)));
        }

        auto Fclass_s(Reg rd, Reg rs1) -> Item
        {
            auto& self = Self();
            auto v = self.Rf(rs1);
            auto bits = static_cast<u32>(v);
            u32 result{};
            if (v == -std::numeric_limits<f32>::infinity())
            {
                // Negative infinity.
                result = 1 << 0;
            }
            else if (v == std::numeric_limits<f32>::infinity())
            {
                // Positive infinity.
                result = 1 << 7;
            }
            else if (bits == 0x80000000)
            {
                // Negative zero.
                result = 1 << 3;
            }
            else if (v == 0.0f)
            {
                // Zero.
                result = 1 << 4;
            }
            else if ((bits & 0x7f800000) == 0)
            {
                // The exponent is zero.
                if ((bits & 0x80000000) != 0)
                {
                    // Negative subnormal number.
                    result = 1 << 2;
                }
                else
                {
                    // Postive subnormal number.
                    result = 1 << 5;
                }
            }
            else if ((bits & 0x7f800000) == 0x7f800000 && (bits & 0x00400000) != 0)
            {
                // Quiet NaN.
                result = 1 << 9;
            }
            else if ((bits & 0x7f800000) == 0x7f800000 && (bits & 0x003fffff) != 0)
            {
                // Signalling NaN.
                result = 1 << 8;
            }
            else if (v < 0.0f)
            {
                // Negative.
                result = 1 << 1;
            }
            else if (v > 0.0f)
            {
                // Positive.
                result = 1 << 6;
            }
            else
            {
                result = 0;
            }
            self.Wx(rd, result);
        }

        auto Fmv_w_x(Reg rd, Reg rs1) -> Item
        {
            // bits(rd) <- bits(rs1)
            auto& self = Self();
            self.Wf(rd, std::bit_cast<f32>(self.Rx(rs1)));
        }

        auto Fsqrt_s(Reg rd, Reg rs1, u32 imm) -> Item
        {
            // rd <- sqrt(rs1)
            auto& self = Self();
            auto f = self.Rf(rs1);
            self.Wf(rd, std::sqrt(f));
        }

        auto Fcvt_w_s(Reg rd, Reg rs1, u32 imm) -> Item
        {
            // rd <- int32_t(rs1)
            auto& self = Self();
            auto i = static_cast<i32>(self.Rf(rs1));
            self.Wx(rd, static_cast<u32>(i));
        }

        auto Fcvt_wu_s(Reg rd, Reg rs1, u32 imm) -> Item
        {
            // rd <- uint32_t(rs1)
            auto& self = Self();
            auto i = static_cast<u32>(self.Rf(rs1));
            self.Wx(rd, i);
        }

        auto Fcvt_s_w(Reg rd, Reg rs1, u32 imm) -> Item
        {
            // rd <- float(int32_t((rs1))
            auto& self = Self();
            auto i = static_cast<i32>(self.Rx(rs1));
            self.Wf(rd, static_cast<f32>(i));
        }

        auto Fcvt_s_wu(Reg rd, Reg rs1, u32 imm) -> Item
        {
            // rd <- float(rs1)
            auto& self = Self();
            self.Wf(rd, static_cast<f32>(self.Rx(rs1)));
        }

        auto Fsgnj_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- abs(rs1) * sgn(rs2)
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wf(rd, std::abs(freg_rs1) * (freg_rs2 < 0.0f ? -1.0f : 1.0f));
        }

        auto Fsgnjn_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- abs(rs1) * -sgn(rs2)
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wf(rd, std::abs(freg_rs1) * (freg_rs2 < 0.0f ? 1.0f : -1.0f));
        }

        auto Fsgnjx_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- abs(rs1) * (sgn(rs1) == sgn(rs2)) ? 1 : -1
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);

            // The sign bit is the XOR of the sign bits of rs1 and rs2.
            auto m = ((freg_rs1 < 0.0f && freg_rs2 >= 0.0f) || (freg_rs1 >= 0.0f && freg_rs2 < 0.0f)) ? -1.0f : 1.0f;
            self.Wf(rd, std::abs(freg_rs1) * m);
        }

        auto Fmin_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- min(rs1, rs2)
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wf(rd, std::min(freg_rs1, freg_rs2));
        }

        auto Fmax_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- max(rs1, rs2)
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wf(rd, std::max(freg_rs1, freg_rs2));
        }

        auto Fle_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- (rs1 <= rs2) ? 1 : 0;
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wx(rd, (freg_rs1 <= freg_rs2) ? 1 : 0);
        }

        auto Flt_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- (rs1 < rs2) ? 1 : 0;
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wx(rd, (freg_rs1 < freg_rs2) ? 1 : 0);
        }

        auto Feq_s(Reg rd, Reg rs1, Reg rs2) -> Item
        {
            // rd <- (rs1 == rs2) ? 1 : 0;
            auto& self = Self();
            auto freg_rs1 = self.Rf(rs1);
            auto freg_rs2 = self.Rf(rs2);
            self.Wx(rd, (freg_rs1 == freg_rs2) ? 1 : 0);
        }

        auto Fadd_s(Reg rd, Reg rs1, Reg rs2, u32 /*rm*/) -> Item
        {
            // rd <- rs1 + rs2
            auto& self = Self();
            self.Wf(rd, self.Rf(rs1) + self.Rf(rs2));
        }

        auto Fsub_s(Reg rd, Reg rs1, Reg rs2, u32 /*rm*/) -> Item
        {
            // rd <- rs1 - rs2
            auto& self = Self();
            self.Wf(rd, self.Rf(rs1) - self.Rf(rs2));
        }

        auto Fmul_s(Reg rd, Reg rs1, Reg rs2, u32 /*rm*/) -> Item
        {
            // rd <- rs1 * rs2
            auto& self = Self();
            self.Wf(rd, self.Rf(rs1) * self.Rf(rs2));
        }

        auto Fdiv_s(Reg rd, Reg rs1, Reg rs2, u32 /*rm*/) -> Item
        {
            // rd <- rs1 / rs2
            auto& self = Self();
            self.Wf(rd, self.Rf(rs1) / self.Rf(rs2));
        }

        auto Flw(Reg rd, Reg rs1, u32 imm) -> Item
        {
            // rd <- f32(rs1 + imm_i)
            auto& self = Self();
            const auto address = self.Rx(rs1) + imm;
            const auto word = self.Read32(address);
            self.Wf(rd, std::bit_cast<f32>(word));
        }

        auto Fsw(Reg rs1, Reg rs2, u32 imm) -> Item
        {
            // f32(rs1 + imm_s) = rs2
            auto& self = Self();
            const auto data = std::bit_cast<u32>(self.Rf(rs2));
            const auto address = self.Rx(rs1) + imm;
            self.Write32(address, data);
        }

        auto Fmadd_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 imm) -> Item
        {
            // rd <- (rs1 * rs2) + rs3
            auto& self = Self();
            self.Wf(rd, (self.Rf(rs1) * self.Rf(rs2)) + self.Rf(rs3));
        }

        auto Fmsub_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 imm) -> Item
        {
            // rd <- (rs1 * rs2) - rs3
            auto& self = Self();
            self.Wf(rd, (self.Rf(rs1) * self.Rf(rs2)) - self.Rf(rs3));
        }

        auto Fnmsub_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 imm) -> Item
        {
            // rd <- -(rs1 * rs2) + rs3
            auto& self = Self();
            self.Wf(rd, -(self.Rf(rs1) * self.Rf(rs2)) + self.Rf(rs3));
        }

        auto Fnmadd_s(Reg rd, Reg rs1, Reg rs2, Reg rs3, u32 imm) -> Item
        {
            // rd <- -(rs1 * rs2) - rs3
            auto& self = Self();
            self.Wf(rd, -(self.Rf(rs1) * self.Rf(rs2)) - self.Rf(rs3));
        }
    };

} // namespace arviss
