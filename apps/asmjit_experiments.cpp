#include <asmjit/asmjit.h>
#include <asmjit/core.h>
#include <cstddef>
#include <format>
#include <iostream>
#include <unordered_map>

enum class Trap : uint32_t
{
    NONE,        // The CPU is not trapped.
    HALT,        // The CPU has been requested to halt.
    BAD_ADDRESS, // The CPU attempted to execute code from a bad address.
};

// Enough of a CPU for experimental purposes.
//
// Instructions supported:
//
// add  - rd <- rs1 + rs2
// addi - rd <- rs1 + imm32
// bne  - pc <- pc + imm32 if rs1 != rs2 else pc + 1
// beq  - pc <- pc + imm32 if rs1 == rs2 else pc + 1
// jmp  - pc <- rs1 + rs2 + imm32

struct Cpu
{
    uint32_t pc;           // The address of the current instruction.
    uint32_t nextPc;       // The address of the next instruction.
    uint32_t xreg[32];     // Integer registers.
    Trap trap{Trap::NONE}; // Set to a value other than Trap::NONE if the CPU is trapped.
};

using Reg = uint32_t;

namespace vm
{
    enum Opcode
    {
        TRAP,
        ADD,
        ADDI,
        BNE,
        BEQ,
        JMP,
    };

    struct Reg3
    {
        Reg r1;
        Reg r2;
        Reg r3;
    };

    struct Reg2Imm
    {
        Reg r1;
        Reg r2;
        int32_t imm;
    };

    struct Instruction
    {
        Opcode op;
        union
        {
            Trap trap;
            struct Reg3 reg3;
            struct Reg2Imm reg2imm;
        };
    };

    using Code = std::vector<Instruction>;

    class Assembler
    {
        Code& code_;

    public:
        Assembler(Code& code) : code_{code} {}

        auto Trap(Trap trap) -> void { code_.emplace_back(Instruction{.op = TRAP, .trap = trap}); }
        auto Add(Reg rd, Reg rs1, Reg rs2) -> void { code_.emplace_back(Instruction{.op = ADD, .reg3 = {.r1 = rd, .r2 = rs1, .r3 = rs2}}); }
        auto Addi(Reg rd, Reg rs1, int32_t imm) -> void { code_.emplace_back(Instruction{.op = ADDI, .reg2imm = {.r1 = rd, .r2 = rs1, .imm = imm}}); }
        auto Bne(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = BNE, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
        auto Beq(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = BEQ, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
        auto Jmp(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = JMP, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
    };
} // namespace vm

// How we invoke a function that works on the CPU.
using CpuFunc = void (*)(Cpu*);

// Let's assume WIN32, x64 for now. Calling convention is that the first argument is in rcx.
constexpr asmjit::x86::Gp ARG0 = asmjit::x86::rcx; // First argument.

inline auto XregOfs(Reg r) -> asmjit::x86::Mem
{
    // What's the address of the given xregister relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, xreg[r])));
}

inline auto PcOfs() -> asmjit::x86::Mem
{
    // What's the address of pc relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, pc)));
}

inline auto NextPcOfs() -> asmjit::x86::Mem
{
    // What's the address of nextPc relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, nextPc)));
}

inline auto TrapOfs() -> asmjit::x86::Mem
{
    // What's the address of trap relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, trap)));
}

class DemoJit
{
    // Runtime designed for JIT - it holds relocated functions and controls their lifetime.
    asmjit::JitRuntime runtime_;

    // Logs errors to stdout.
    asmjit::FileLogger logger_ = asmjit::FileLogger(stdout);

    // Holds code and relocation information during code generation.
    asmjit::CodeHolder code_;

    // An assembler that can emit code to the code holder.
    asmjit::x86::Assembler a_;

    // A map from VM addresses to their code offsets in the generated function.
    std::unordered_map<uint32_t, CpuFunc> offsetMap_{};

    // Pending offsets, yet to be resolved to addresses and added to the map.
    using OffsetPair = std::pair<uint32_t, size_t>;
    std::vector<OffsetPair> pendingOffsets_{};

    uint32_t pc_{};

public:
    DemoJit()
    {
        // Initialize the code holder so that it can be used.
        code_.init(runtime_.environment(), runtime_.cpuFeatures());
        code_.setLogger(&logger_);
        code_.attach(&a_);
    }

    auto SetPc(uint32_t pc) -> void { pc_ = pc; }

    // Adds an offset whose address we need to fix up later.
    auto AddOffset() -> uint32_t
    {
        pendingOffsets_.emplace_back(OffsetPair(pc_, a_.offset()));
        const auto oldPc = pc_;
        pc_ += 1;
        return oldPc;
    };

    // Resolves a VM address into a native address.
    auto Resolve(uint32_t vmAddr) -> CpuFunc { return offsetMap_[vmAddr]; }

    auto Compile() -> CpuFunc
    {
        // The assembler is no longer needed from here, so detach it from the code holder.
        code_.detach(&a_);

        // Copy and relocate the generated code from the code holder to the JitRuntime.
        CpuFunc generatedFunc;
        if (asmjit::Error err = runtime_.add(&generatedFunc, &code_))
        {
            std::cerr << "AsmJit failed: " << asmjit::DebugUtils::errorAsString(err) << '\n';
            // TODO: error handling.
        }

        // Fix up those offsets so that we have a direct mapping from VM addresses to native addresses.
        const auto baseAddress = code_.baseAddress();
        std::cout << std::format("Base address of compiled code: 0x{:08x}\n", baseAddress);
        for (auto [vmAddr, offset] : pendingOffsets_)
        {
            const std::uintptr_t addr = baseAddress + offset;
            std::cout << std::format("vm address 0x{:04x} is native address 0x{:08x}\n", vmAddr, addr);
            offsetMap_[vmAddr] = asmjit::ptr_as_func<CpuFunc>(reinterpret_cast<void*>(addr));
        }

        // Reset so that we're ready for the next round of compilation.
        // TODO: there's probably a nicer way of doing this.
        pendingOffsets_.clear();
        code_.reset();
        code_.init(runtime_.environment(), runtime_.cpuFeatures());
        code_.setLogger(&logger_);
        code_.attach(&a_);

        // Usual caveats about lifetimes.
        return generatedFunc;
    }

    // Returns from JITted code to the execution environment.
    auto EmitReturnToEE() -> void { a_.ret(); }

    // Signals a trap on the CPU.
    auto EmitTrap(Trap trap) -> uint32_t
    {
        const auto pc = AddOffset();

        const auto addr = TrapOfs();
        a_.mov(asmjit::x86::eax, static_cast<uint32_t>(trap));
        a_.mov(addr, asmjit::x86::eax);
        EmitReturnToEE();

        return pc;
    }

    // Adds two registers and stores the result in a third.
    auto EmitAdd(Reg rd, Reg rs1, Reg rs2) -> uint32_t
    {
        const auto pc = AddOffset();

        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);
        const auto addrRd = XregOfs(rd);

        // Only emit code if we're not writing to x0, as that's always zero.
        if (rd != 0)
        {
            a_.mov(asmjit::x86::eax, addrRs1);
            a_.add(asmjit::x86::eax, addrRs2);
            a_.mov(addrRd, asmjit::x86::eax);
        }

        return pc;
    }

    // Adds a register and an immediate value and stores the result in a destination register.
    auto EmitAddi(Reg rd, Reg rs1, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRd = XregOfs(rd);

        // Only emit code if we're not writing to rd, as that's always zero.
        if (rd != 0)
        {
            // Add if rs1 is not x0, otherwise just move.
            if (rs1 != 0)
            {
                a_.mov(asmjit::x86::eax, addrRs1);
                a_.add(asmjit::x86::eax, imm);
            }
            else
            {
                a_.mov(asmjit::x86::eax, imm);
            }
            a_.mov(addrRd, asmjit::x86::eax);
        }

        return pc;
    }

    // Compares two registers and branches if they aren't equal.
    auto EmitBne(Reg rs1, Reg rs2, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);
        const asmjit::Label branchNotTaken = a_.newLabel();

        // TODO: what if it's the same register?
        a_.mov(asmjit::x86::eax, addrRs1); // TODO: what if this was x0 ?
        a_.cmp(asmjit::x86::eax, addrRs2); // TODO: what if this was x0 ?
        a_.je(branchNotTaken);             // Ironically.

        // We took the branch. nextPc <- pc + imm
        a_.mov(asmjit::x86::eax, pc + imm);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        EmitReturnToEE();

        // We didn't take the branch. nextPc <- pc + 1
        a_.bind(branchNotTaken);
        a_.mov(asmjit::x86::eax, pc + 1);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        EmitReturnToEE();

        return pc;
    }

    // Compares two registers and branches if they are equal.
    auto EmitBeq(Reg rs1, Reg rs2, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);
        const asmjit::Label branchNotTaken = a_.newLabel();

        // TODO: what if it's the same register?
        a_.mov(asmjit::x86::eax, addrRs1); // TODO: what if this was x0 ?
        a_.cmp(asmjit::x86::eax, addrRs2); // TODO: what if this was x0 ?
        a_.jne(branchNotTaken);            // Ironically.

        // We took the branch. nextPc <- pc + imm
        a_.mov(asmjit::x86::eax, pc + imm);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        EmitReturnToEE();

        // We didn't take the branch. nextPc <- pc + 1
        a_.bind(branchNotTaken);
        a_.mov(asmjit::x86::eax, pc + 1);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        EmitReturnToEE();

        return pc;
    }

    // Do an indirect jmp ro [rs1 + rs2 + imm].
    auto EmitJmp(Reg rs1, Reg rs2, int32_t imm) -> uint32_t
    {
        const auto pc = AddOffset();
        const auto addrRs1 = XregOfs(rs1);
        const auto addrRs2 = XregOfs(rs2);

        a_.mov(asmjit::x86::eax, addrRs1); // TODO: what if this was x0 ?
        a_.add(asmjit::x86::eax, addrRs2); // TODO: what if this was x0 ?
        a_.add(asmjit::x86::eax, imm);     // TODO: what if this was zero ?
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        EmitReturnToEE();

        return pc;
    }

    auto JitBlock(const vm::Code& code, uint32_t pc) -> CpuFunc
    {
        std::cout << std::format("Compiling from 0x{:04x}\n", pc);
        if (pc >= code.size())
        {
            return nullptr;
        }
        SetPc(pc);
        bool isEndOfBasicBlock = false;
        for (auto it = code.cbegin() + pc; it != code.cend() && !isEndOfBasicBlock; ++it)
        {
            const auto& ins = *it;
            switch (ins.op)
            {
            case vm::TRAP:
                EmitTrap(ins.trap);
                isEndOfBasicBlock = true;
                break;
            case vm::ADD:
                EmitAdd(ins.reg3.r1, ins.reg3.r2, ins.reg3.r3);
                break;
            case vm::ADDI:
                EmitAddi(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                break;
            case vm::BNE:
                EmitBne(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                isEndOfBasicBlock = true;
                break;
            case vm::BEQ:
                EmitBeq(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                isEndOfBasicBlock = true;
                break;
            case vm::JMP:
                EmitJmp(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
                isEndOfBasicBlock = true;
                break;
            }
        }

        return isEndOfBasicBlock ? Compile() : nullptr;
    }
};

class System
{
    vm::Code code_{};
    Cpu cpu_{};
    DemoJit jit_{};

    auto Resolve(uint32_t pc) -> CpuFunc
    {
        // If the JIT already knows about the address then return it.
        CpuFunc func = jit_.Resolve(pc);
        if (func)
        {
            return func;
        }

        // If the JIT doesn't know about the address then attempt to JIT a basic block.
        func = jit_.JitBlock(code_, pc);

        // If that fails, then we have a bad address, so signal a trap on the CPU.
        if (func == nullptr)
        {
            cpu_.trap = Trap::BAD_ADDRESS;
        }

        return func;
    }

public:
    System(vm::Code&& code) : code_{std::move(code)} {}

    auto Run() -> void
    {
        // JIT the VM's code, one basic block at a time, and run it.
        uint32_t pc = 0;
        CpuFunc func = nullptr;
        for (func = Resolve(pc); func != nullptr && cpu_.trap == Trap::NONE; func = Resolve(pc))
        {
            // Call the native code.
            func(&cpu_);
            std::cout << "x1 = " << cpu_.xreg[1] << '\n';

            // Get the VM address of the next instruction.
            pc = cpu_.nextPc;

            std::cout << "pc = " << pc << '\n'; // TODO: remove.
        }
        std::cout << "Execution ended with status: ";
        switch (cpu_.trap)
        {
        case Trap::NONE:
            std::cout << "Success";
            break;
        case Trap::BAD_ADDRESS:
            std::cout << std::format("Bad Address: 0x{:04x}", pc);
            break;
        case Trap::HALT:
            std::cout << "Halt";
        }
        std::cout << '\n';
    }
};

auto main() -> int
{
    try
    {
        // Assemble some VM instructions into `code`.
        vm::Code code;
        vm::Assembler a(code);

        // Basic block. Add a few things together and fall through.
        a.Add(1, 2, 3); // 0: add x1, x2, x3
        a.Add(0, 1, 1); // 1: add x0, x1, x1
        a.Add(2, 2, 2); // 2: add x2, x2, x2
        a.Beq(0, 0, 1); // 3: beq x0, x0, +1

        // Basic block. A loop that counts down from 10 to 0.
        a.Addi(1, 0, 10); // 4: addi x1, 0, 10
        a.Addi(1, 1, -1); // 5: addi x1, x1, -1
        a.Bne(1, 0, -1);  // 6: bne x1, x0, -1

        // Basic block. Set up a few registers.
        a.Addi(1, 0, 15360); //  7: addi x1, 0, 3c00h
        a.Addi(2, 0, 15361); //  8: addi x2, 0, 3c01h
        a.Addi(3, 0, 1023);  //  9: addi x3, 0, 3ffh
        a.Beq(0, 0, 1);      // 10: beq x0, x0, +1

        // Basic block. A loop that counts down from 10 to 0.
        a.Addi(1, 0, 10); // 11: addi x1, x0, 10
        a.Addi(1, 1, -1); // 12: addi x1, x1, -1
        a.Bne(1, 0, -1);  // 13: bne x1, x0, -1

        // // Basic Block. Do an indirect jump to x1 + x0 + 0.
        a.Addi(1, 0, 16); // 14: addi x1, x0, 16
        a.Jmp(1, 0, 0);   // 15: jmp x1 + x0 + 0

        // Basic block. Load a value into x1. Halt. Do not catch fire.
        a.Addi(1, 0, 1337); // 16: addi x1, x0, 1337
        a.Trap(Trap::HALT); // 17: trap halt

        // Move the code into a system that can run it.
        System system(std::move(code));
        system.Run();

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
