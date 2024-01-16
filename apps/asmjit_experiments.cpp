#include <asmjit/asmjit.h>
#include <asmjit/core.h>
#include <cstddef>
#include <format>
#include <iostream>
#include <unordered_map>

// Enough of a CPU for experimental purposes.
//
// Instructions supported:
//
// add  - rd <- rs1 + rs2
// addi - rd <- rs1 + imm32
// bne  - pc <- pc + imm32 if rs1 != rs2 else pc + 1
// beq  - pc <- pc + imm32 if rs1 == rs2 else pc + 1

struct Cpu
{
    uint32_t pc;       // The address of the current instruction.
    uint32_t nextPc;   // The address of the next instruction.
    uint32_t xreg[32]; // Integer registers.
    bool isTrapped;    // Is the CPU trapped.
};

using Reg = uint32_t;

namespace vm
{
    enum Opcode
    {
        HALT,
        ADD,
        ADDI,
        BNE,
        BEQ,
    };

    struct Empty
    {
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
            struct Empty empty;
            struct Reg3 reg3;
            struct Reg2Imm reg2imm;
        };
    };

    struct Machine
    {
        std::vector<Instruction> code_{};

        auto Halt() -> void { code_.emplace_back(Instruction{.op = HALT, .empty = {}}); }
        auto Add(Reg rd, Reg rs1, Reg rs2) -> void { code_.emplace_back(Instruction{.op = ADD, .reg3 = {.r1 = rd, .r2 = rs1, .r3 = rs2}}); }
        auto Addi(Reg rd, Reg rs1, int32_t imm) -> void { code_.emplace_back(Instruction{.op = ADDI, .reg2imm = {.r1 = rd, .r2 = rs1, .imm = imm}}); }
        auto Bne(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = BNE, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
        auto Beq(Reg rs1, Reg rs2, int32_t imm) -> void { code_.emplace_back(Instruction{.op = BEQ, .reg2imm{.r1 = rs1, .r2 = rs2, .imm = imm}}); }
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

inline auto IsTrappedOfs() -> asmjit::x86::Mem
{
    // What's the address of isTrapped relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, isTrapped)));
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

    // Ends a function and returns to the execution environment.
    auto EmitEndFunc() -> uint32_t
    {
        const auto pc = AddOffset();
        a_.ret();
        return pc;
    }

    auto EmitTrap() -> uint32_t
    {
        const auto pc = AddOffset();

        const auto addr = IsTrappedOfs();
        a_.mov(asmjit::x86::eax, 1);
        a_.mov(addr, asmjit::x86::eax);
        a_.ret();

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
        a_.ret(); // Return to the execution environment.

        // We didn't take the branch. nextPc <- pc + 1
        a_.bind(branchNotTaken);
        a_.mov(asmjit::x86::eax, pc + 1);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        a_.ret(); // Return to the execution environment.

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
        a_.ret(); // Return to the execution environment.

        // We didn't take the branch. nextPc <- pc + 1
        a_.bind(branchNotTaken);
        a_.mov(asmjit::x86::eax, pc + 1);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        a_.ret(); // Return to the execution environment.

        return pc;
    }
};

auto JitBlock(DemoJit& jit, vm::Machine& machine, uint32_t pc) -> CpuFunc
{
    std::cout << std::format("Compiling from 0x{:04x}\n", pc);
    jit.SetPc(pc);
    bool isEndOfBasicBlock = false;
    for (size_t index = pc; !isEndOfBasicBlock && index < machine.code_.size(); index++)
    {
        const auto& ins = machine.code_[index];
        switch (ins.op)
        {
        case vm::HALT:
            jit.EmitTrap();
            isEndOfBasicBlock = true;
            break;
        case vm::ADD:
            jit.EmitAdd(ins.reg3.r1, ins.reg3.r2, ins.reg3.r3);
            break;
        case vm::ADDI:
            jit.EmitAddi(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
            break;
        case vm::BNE:
            jit.EmitBne(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
            isEndOfBasicBlock = true;
            break;
        case vm::BEQ:
            jit.EmitBeq(ins.reg2imm.r1, ins.reg2imm.r2, ins.reg2imm.imm);
            isEndOfBasicBlock = true;
            break;
        }
    }
    return isEndOfBasicBlock ? jit.Compile() : nullptr;
}

auto Resolve(DemoJit& jit, vm::Machine& machine, uint32_t pc) -> CpuFunc
{
    CpuFunc func = jit.Resolve(pc);
    if (func == nullptr)
    {
        func = JitBlock(jit, machine, pc);
    }
    return func;
}

auto main() -> int
{
    try
    {
        // Create a VM and load it up with some instructions. This is what we'll be jitting *from*. The instructions
        // are simple and would have little to no decoding overhead if we actually decoded them.
        vm::Machine vm;

        // Basic block. Add a few things together and fall through.
        vm.Add(1, 2, 3); // 0
        vm.Add(0, 1, 1); // 1
        vm.Add(2, 2, 2); // 2
        vm.Beq(0, 0, 1); // 3

        // Basic block. A loop that counts down from 10 to 0.
        vm.Addi(1, 0, 10); // 4
        vm.Addi(1, 1, -1); // 5
        vm.Bne(1, 0, -1);  // 6

        // Basic block. Set up a few registers.
        vm.Addi(1, 0, 15360); // 7
        vm.Addi(2, 0, 15361); // 8
        vm.Addi(3, 0, 1023);  // 9
        vm.Beq(0, 0, 1);      // 10

        // Basic block. A loop that counts down from 10 to 0.
        vm.Addi(1, 0, 10); // 11
        vm.Addi(1, 1, -1); // 12
        vm.Bne(1, 0, -1);  // 13

        // Basic block. Halt. Do not catch fire.
        vm.Halt(); // 14

        // Create a JIT.
        auto jit = DemoJit();

        // Create a CPU.
        Cpu cpu{};

        // JIT the VM's code, one basic block at a time, and run it.
        uint32_t pc = 0;
        for (CpuFunc func = Resolve(jit, vm, pc); func != nullptr && !cpu.isTrapped; func = Resolve(jit, vm, pc))
        {
            // Call the native code.
            func(&cpu);
            std::cout << "x1 = " << cpu.xreg[1] << '\n';

            // Get the VM address of the next instruction.
            pc = cpu.nextPc;
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
