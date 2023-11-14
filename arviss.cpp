// A RISC-V instruction set simulator implemented using C++ concepts, using a similar style to the use of Rust traits in
// https://github.dev/badlydrawnrod/arviss_experiment.

#include <array>
#include <concepts>
#include <cstdint>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using Address = std::uint32_t;
using Reg = std::uint32_t;
using u8 = std::uint8_t;
using u16 = std::uint16_t;
using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using u32 = std::uint32_t;
using f32 = float;

// T is an instruction handler for Rv32i instructions whose member functions do not return a value.
template<typename T>
concept IsVoidRv32iInstructionHandler = std::same_as<void, typename T::Item> && requires(T self) {
    self.Ecall();
    self.Ebreak();
    self.Add(Reg{}, Reg{}, Reg{});
    self.Sub(Reg{}, Reg{}, Reg{});
    self.Sll(Reg{}, Reg{}, Reg{});
    self.Slt(Reg{}, Reg{}, Reg{});
    self.Sltu(Reg{}, Reg{}, Reg{});
    self.Xor(Reg{}, Reg{}, Reg{});
    self.Srl(Reg{}, Reg{}, Reg{});
    self.Sra(Reg{}, Reg{}, Reg{});
    self.Or(Reg{}, Reg{}, Reg{});
    self.And(Reg{}, Reg{}, Reg{});
    self.Slli(Reg{}, Reg{}, u32{});
    self.Srli(Reg{}, Reg{}, u32{});
    self.Srai(Reg{}, Reg{}, u32{});
    self.Beq(Reg{}, Reg{}, u32{});
    self.Bne(Reg{}, Reg{}, u32{});
    self.Blt(Reg{}, Reg{}, u32{});
    self.Bge(Reg{}, Reg{}, u32{});
    self.Bltu(Reg{}, Reg{}, u32{});
    self.Bgeu(Reg{}, Reg{}, u32{});
    self.Jalr(Reg{}, Reg{}, u32{});
    self.Addi(Reg{}, Reg{}, u32{});
    self.Slti(Reg{}, Reg{}, u32{});
    self.Sltiu(Reg{}, Reg{}, u32{});
    self.Xori(Reg{}, Reg{}, u32{});
    self.Ori(Reg{}, Reg{}, u32{});
    self.Andi(Reg{}, Reg{}, u32{});
    self.Lb(Reg{}, Reg{}, u32{});
    self.Lh(Reg{}, Reg{}, u32{});
    self.Lw(Reg{}, Reg{}, u32{});
    self.Lbu(Reg{}, Reg{}, u32{});
    self.Lhu(Reg{}, Reg{}, u32{});
    self.Sb(Reg{}, Reg{}, u32{});
    self.Sh(Reg{}, Reg{}, u32{});
    self.Sw(Reg{}, Reg{}, u32{});
    self.Fence(u32{}, Reg{}, Reg{});
    self.Jal(Reg{}, u32{});
    self.Lui(Reg{}, u32{});
    self.Auipc(Reg{}, u32{});
};

// T is an instruction handler for Rv32i instructions whose member functions return a value.
template<typename T>
concept IsNonVoidRv32iInstructionHandler = !std::same_as<void, typename T::Item> && requires(T self, typename T::Item item) {
    item = self.Ecall();
    item = self.Ebreak();
    item = self.Add(Reg{}, Reg{}, Reg{});
    item = self.Sub(Reg{}, Reg{}, Reg{});
    item = self.Sll(Reg{}, Reg{}, Reg{});
    item = self.Slt(Reg{}, Reg{}, Reg{});
    item = self.Sltu(Reg{}, Reg{}, Reg{});
    item = self.Xor(Reg{}, Reg{}, Reg{});
    item = self.Srl(Reg{}, Reg{}, Reg{});
    item = self.Sra(Reg{}, Reg{}, Reg{});
    item = self.Or(Reg{}, Reg{}, Reg{});
    item = self.And(Reg{}, Reg{}, Reg{});
    item = self.Slli(Reg{}, Reg{}, u32{});
    item = self.Srli(Reg{}, Reg{}, u32{});
    item = self.Srai(Reg{}, Reg{}, u32{});
    item = self.Beq(Reg{}, Reg{}, u32{});
    item = self.Bne(Reg{}, Reg{}, u32{});
    item = self.Blt(Reg{}, Reg{}, u32{});
    item = self.Bge(Reg{}, Reg{}, u32{});
    item = self.Bltu(Reg{}, Reg{}, u32{});
    item = self.Bgeu(Reg{}, Reg{}, u32{});
    item = self.Jalr(Reg{}, Reg{}, u32{});
    item = self.Addi(Reg{}, Reg{}, u32{});
    item = self.Slti(Reg{}, Reg{}, u32{});
    item = self.Sltiu(Reg{}, Reg{}, u32{});
    item = self.Xori(Reg{}, Reg{}, u32{});
    item = self.Ori(Reg{}, Reg{}, u32{});
    item = self.Andi(Reg{}, Reg{}, u32{});
    item = self.Lb(Reg{}, Reg{}, u32{});
    item = self.Lh(Reg{}, Reg{}, u32{});
    item = self.Lw(Reg{}, Reg{}, u32{});
    item = self.Lbu(Reg{}, Reg{}, u32{});
    item = self.Lhu(Reg{}, Reg{}, u32{});
    item = self.Sb(Reg{}, Reg{}, u32{});
    item = self.Sh(Reg{}, Reg{}, u32{});
    item = self.Sw(Reg{}, Reg{}, u32{});
    item = self.Fence(u32{}, Reg{}, Reg{});
    item = self.Jal(Reg{}, u32{});
    item = self.Lui(Reg{}, u32{});
    item = self.Auipc(Reg{}, u32{});
};

// T is an instruction handler for Rv32i instructions.
template<typename T>
concept IsRv32iInstructionHandler = IsNonVoidRv32iInstructionHandler<T> || IsVoidRv32iInstructionHandler<T>;

// T is an instruction dispatcher.
template<typename T>
concept IsDispatcher = requires(T t) {
    {
        t.Dispatch(u32{})
    } -> std::same_as<typename T::Item>;
};

// T is an instruction dispatcher for Rv32i instruction handlers.
template<typename T>
concept IsIntDispatcher = IsDispatcher<T> && IsRv32iInstructionHandler<T>;

// T supports reading from and writing to integer registers.
template<typename T>
concept HasXRegisters = requires(T t, u32 result) {
    result = t.Rx(Reg{});
    t.Wx(Reg{}, u32{});
};

// T supports reading from and writing to memory.
template<typename T>
concept HasMemory = requires(T t, u8 b, u16 h, u32 w) {
    b = t.Read8(Address{});
    h = t.Read16(Address{});
    w = t.Read32(Address{});
    t.Write8(Address{}, u8{});
    t.Write16(Address{}, u16{});
    t.Write32(Address{}, u32{});
};

// T implements the fetch cycle.
template<typename T>
concept HasFetch = requires(T t, Address a, u32 r) {
    a = t.Pc();               // Returns the contents of the program counter.
    a = t.Transfer();         // Transfers nextPc to the program counter and returns it.
    r = t.Fetch();            // Transfer, Fetch32(pc), SetNextPc, return instruction.
    t.SetNextPc(Address{});   // Sets nextPc.
    r = t.Fetch32(Address{}); // Returns the instruction at the given address.
};

// T has all the pieces of an integer CPU.
template<typename T>
concept IsIntCpu = HasXRegisters<T> // it has integer registers
        && HasFetch<T>              // it has a fetch cycle implementation
        && HasMemory<T>;            // it has memory

// T is a VM capable of fetching, dispatching and handling integer instructions for an integer CPU.
template<typename T>
concept IsIntVm = IsDispatcher<T> && IsRv32iInstructionHandler<T> && IsIntCpu<T>;

// T is a dispatcher for a tracing handler for Rv32i instructions.
template<typename T>
concept IsIntTrace = IsDispatcher<T> && IsRv32iInstructionHandler<T> && std::same_as<std::string, typename T::Item>;

// A helper for breaking down instructions.
class Instruction
{
    u32 ins_{};

    auto Bits(u32 hi, u32 lo) -> u32
    {
        auto n = ins_;
        auto run = (hi - lo) + 1;
        auto mask = ((1 << run) - 1) << lo;
        return (n & mask) >> lo;
    }

public:
    Instruction(u32 ins) : ins_{ins} {};

    auto Opcode() -> u32 const { return ins_ & 0x7f; }

    auto Rd() -> u32 const { return (ins_ >> 7) & 0x1f; }
    auto Rs1() -> u32 const { return (ins_ >> 15) & 0x1f; }
    auto Rs2() -> u32 const { return (ins_ >> 20) & 0x1f; }

    auto Shamtw() -> u32 const { return (ins_ >> 20) & 0x1f; }

    auto Fm() -> u32 const { return (ins_ >> 20) & 0xf; }

    auto BImmediate() -> u32 const
    {
        auto imm12 = static_cast<i32>(ins_ & 0x80000000) >> 19;     // ins[31] -> sext(imm[12])
        auto imm11 = static_cast<i32>((ins_ & 0x00000080) << 4);    // ins[7] -> imm[11]
        auto imm10_5 = static_cast<i32>((ins_ & 0x7e000000) >> 20); // ins[30:25] -> imm[10:5]
        auto imm4_1 = static_cast<i32>((ins_ & 0x00000f00) >> 7);   // ins[11:8]  -> imm[4:1]
        return static_cast<u32>(imm12 | imm11 | imm10_5 | imm4_1);
    }

    auto IImmediate() -> u32 const
    {
        return static_cast<u32>((static_cast<i32>(ins_) >> 20)); // ins[31:20] -> sext(imm[11:0])
    }

    auto SImmediate() -> u32 const
    {
        auto imm11_5 = (static_cast<i32>(ins_ & 0xfe000000)) >> 20; // ins[31:25] -> sext(imm[11:5])
        auto imm4_0 = static_cast<i32>((ins_ & 0x00000f80) >> 7);   // ins[11:7]  -> imm[4:0]
        return static_cast<u32>(imm11_5 | imm4_0);
    }

    auto JImmediate() -> u32 const
    {
        auto imm20 = static_cast<i32>(ins_ & 0x80000000) >> 11;     // ins[31] -> sext(imm[20])
        auto imm19_12 = static_cast<i32>(ins_ & 0x000ff000);        // ins[19:12] -> imm[19:12]
        auto imm11 = static_cast<i32>((ins_ & 0x00100000) >> 9);    // ins[20] -> imm[11]
        auto imm10_1 = static_cast<i32>((ins_ & 0x7fe00000) >> 20); // ins[30:21] -> imm[10:1]
        return static_cast<u32>(imm20 | imm19_12 | imm11 | imm10_1);
    }

    auto UImmediate() -> u32 const
    {
        return ins_ & 0xfffff000; // ins[31:12] -> imm[31:12]
    }
};

// An instruction dispatcher for Rv32i instruction handlers.
template<typename Handler>
    requires IsRv32iInstructionHandler<Handler>
struct MRv32iDispatcher : public Handler
{
    using Item = Handler::Item;

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
            case 0x00000063: return self.Beq(c.Rs1(), c.Rs2(), c.BImmediate());
            case 0x00001063: return self.Bne(c.Rs1(), c.Rs2(), c.BImmediate());
            case 0x00004063: return self.Blt(c.Rs1(), c.Rs2(), c.BImmediate());
            case 0x00005063: return self.Bge(c.Rs1(), c.Rs2(), c.BImmediate());
            case 0x00006063: return self.Bltu(c.Rs1(), c.Rs2(), c.BImmediate());
            case 0x00007063: return self.Bgeu(c.Rs1(), c.Rs2(), c.BImmediate());
            case 0x00000067: return self.Jalr(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00000013: return self.Addi(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00002013: return self.Slti(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00003013: return self.Sltiu(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00004013: return self.Xori(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00006013: return self.Ori(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00007013: return self.Andi(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00000003: return self.Lb(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00001003: return self.Lh(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00002003: return self.Lw(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00004003: return self.Lbu(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00005003: return self.Lhu(c.Rd(), c.Rs1(), c.IImmediate());
            case 0x00000023: return self.Sb(c.Rs1(), c.Rs2(), c.SImmediate());
            case 0x00001023: return self.Sh(c.Rs1(), c.Rs2(), c.SImmediate());
            case 0x00002023: return self.Sw(c.Rs1(), c.Rs2(), c.SImmediate());
            case 0x0000000f: return self.Fence(c.Fm(), c.Rd(), c.Rs1());
        }
        switch (code & 0x0000007f) {
            case 0x0000006f: return self.Jal(c.Rd(), c.JImmediate());
            case 0x00000037: return self.Lui(c.Rd(), c.UImmediate());
            case 0x00000017: return self.Auipc(c.Rd(), c.UImmediate());
        }
        return self.Illegal(code);
    }
    // clang-format on
};

// A mixin implementation of RV32i's integer registers.
class MXRegisters
{
    std::array<u32, 32> xreg_{};

public:
    auto Rx(Reg rs) -> u32 { return xreg_[rs]; }

    auto Wx(Reg rd, u32 val) -> void
    {
        xreg_[rd] = val;
        xreg_[0] = 0;
    }
};

// A mixin implementation of simple, unchecked memory.
class MBasicMem
{
    // 32KiB of memory.
    std::array<u8, 32768> mem_{};

    // Memory mapped I/O.
    const Address TTY_STATUS = 0x8000;
    const Address TTY_DATA = 0x8001;

public:
    auto Read8(Address address) -> u8
    {
        if (address != TTY_STATUS) { return mem_[address]; }
        return 1;
    }

    auto Read16(Address address) -> u16
    {
        auto* p = reinterpret_cast<u16*>(&mem_[address]);
        return *p;
    }

    auto Read32(Address address) -> u32
    {
        auto* p = reinterpret_cast<u32*>(&mem_[address]);
        return *p;
    }

    auto Write8(Address address, u8 byte) -> void
    {
        if (address == TTY_DATA) { std::cout << static_cast<char>(byte); }
        else { mem_[address] = byte; }
    }

    auto Write16(Address address, u16 halfWord) -> void
    {
        auto* p = reinterpret_cast<u16*>(&mem_[address]);
        *p = halfWord;
    }

    auto Write32(Address address, u32 word) -> void
    {
        auto* p = reinterpret_cast<u32*>(&mem_[address]);
        *p = word;
    }
};

// A mixin implementation of the fetch cycle for the given memory implementation.
template<HasMemory Mem>
class MFetch : public Mem
{
    Address pc_{};
    Address nextPc_{};

public:
    auto Pc() -> Address const { return pc_; }

    auto Transfer() -> Address
    {
        pc_ = nextPc_;
        return pc_;
    }

    auto Fetch() -> u32
    {
        auto pc = Transfer();
        auto ins = Fetch32(pc);
        SetNextPc(pc + 4);
        return ins;
    }

    auto SetNextPc(Address address) -> void { nextPc_ = address; }

    auto Fetch32(Address address) -> u32
    {
        auto& mem_ = static_cast<Mem&>(*this);
        return mem_.Read32(address);
    }
};

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

// A mixin CPU with registers and basic memory with a fetch cycle implementation.
struct MIntCpu : public MXRegisters,      // it has registers
                 public MFetch<MBasicMem> // it has memory with a fetch cycle implemented on it
{
};

// An RV32i CPU with registers, basic memory with a fetch cycle implementation, a handler for integer instructions, and
// a dispatcher.
template<IsIntCpu T>
using RV32iCpu = MRv32iDispatcher<MRv32iHandler<T>>;

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

// This works for IsFloatVm too.
template<IsIntVm T>
auto Run(T& t, size_t count) -> void
{
    while (count > 0)
    {
        auto ins = t.Fetch(); // Fetch.
        t.Dispatch(ins);      // Execute.
        --count;
    }
}

// How do I say that U's instruction handler can't be a subset of T's instruction handler?
template<IsIntVm T, IsIntTrace U>
auto Run(T& t, U& u, size_t count) -> void
{
    while (count > 0)
    {
        auto ins = t.Fetch();                                                    // Fetch.
        std::cout << std::format("{:04x}\t", t.Pc()) << u.Dispatch(ins) << '\n'; // Trace.
        t.Dispatch(ins);                                                         // Execute.
        --count;
    }
}

auto main() -> int
{
    try
    {
        // Read the image into a buffer.
        std::ifstream fileHandler("images/hello_world.rv32i", std::ios::in | std::ios::binary | std::ios::ate);
        const size_t fileSize = fileHandler.tellg();
        fileHandler.seekg(0, std::ios::beg);
        std::vector<u8> buf(fileSize);
        fileHandler.read(reinterpret_cast<char*>(buf.data()), fileSize);
        fileHandler.close();

        // Create a CPU.
        RV32iCpu<MIntCpu> cpu;

        // Populate its memory with the contents of the image.
        Address addr = 0;
        for (auto b : buf)
        {
            cpu.Write8(addr, b);
            ++addr;
        }

        // Trace the execution.
        // Disassembler dis;
        // Run(cpu, dis, 10000);

        // Execute some instructions. We should see "Hello world from Rust!" because that's what compiled the image.
        Run(cpu, 10000);
    }
    catch (const std::exception& e)
    {
        std::cerr << "VM exited with: " << e.what() << '\n';
    }
}
