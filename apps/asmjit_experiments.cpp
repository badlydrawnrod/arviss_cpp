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
// bne  - pc <- pc + imm32 if rs1 != rs2 else pc + 4

struct Cpu
{
    uint32_t pc;       // The address of the current instruction.
    uint32_t nextPc;   // The address of the next instruction.
    uint32_t xreg[32]; // Integer registers.
};

// How we invoke a function that works on the CPU.
using CpuFunc = void (*)(Cpu*);

using Reg = uint32_t;

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

    // Adds an offset whose address we need to fix up later.
    auto AddOffset() -> uint32_t
    {
        pendingOffsets_.emplace_back(OffsetPair(pc_, a_.offset()));
        const auto oldPc = pc_;
        pc_ += 4;
        return oldPc;
    };

    // Resolves a VM address into a native address.
    auto Resolve(uint32_t vmAddr) -> CpuFunc { return offsetMap_.at(vmAddr); }

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
        std::cout << std::format("Base address: 0x{:08x}\n", baseAddress);
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

        // We didn't take the branch. nextPc <- pc + 4
        a_.bind(branchNotTaken);
        a_.mov(asmjit::x86::eax, pc + 4);
        a_.mov(NextPcOfs(), asmjit::x86::eax);
        a_.ret(); // Return to the execution environment.

        return pc;
    }
};

auto main() -> int
{
    try
    {
        // Create a JIT.
        auto jit = DemoJit();

        // Emit a straightforward function that adds a few things then returns.
        jit.EmitAdd(1, 2, 3);    // add x1, x2, x3
        jit.EmitAdd(0, 1, 1);    // add x0, x1, x1
        jit.EmitAdd(2, 2, 2);    // add x2, x2, x2
        jit.EmitAddi(3, 0, 100); // addi x3, x0, 100
        jit.EmitEndFunc();

        // Emit a loop that counts down from 10 to 0.
        auto loopEntryOfs = jit.EmitAddi(1, 0, 10); // add x1, x0, 10
        jit.EmitAddi(1, 1, -1);                     // add x1, x1, -1
        jit.EmitBne(1, 0, -4);                      // bne x1, x0, -4
        const auto endPc = jit.EmitEndFunc();

        CpuFunc simpleFunc = jit.Compile();

        // Call the first piece of code.
        Cpu cpu{};
        cpu.xreg[1] = 1234;
        cpu.xreg[2] = 20;
        cpu.xreg[3] = 22;
        std::cout << "xreg[1] before calling compiled code is: " << cpu.xreg[1] << '\n';
        std::cout << "xreg[2] before calling compiled code is: " << cpu.xreg[2] << '\n';
        simpleFunc(&cpu);
        std::cout << "xreg[1] after calling compiled code is: " << cpu.xreg[1] << '\n'; // 42   (from add x1, x2, x3)
        std::cout << "xreg[2] after calling compiled code is: " << cpu.xreg[2] << '\n'; // 40   (from add x2, x2, x2)
        std::cout << "xreg[3] after calling compiled code is: " << cpu.xreg[3] << '\n'; // 100  (from addi, x3, x0, 100)

        // Now to live dangerously. Call the second piece of code, and use the value of nextPc to figure out where to
        // go next.
        auto callFunc = jit.Resolve(loopEntryOfs);
        callFunc(&cpu);
        std::cout << "xreg[1] is: " << cpu.xreg[1] << '\n';
        while (cpu.nextPc != endPc)
        {
            // Resolve the address of the next piece of compiled code.
            callFunc = jit.Resolve(cpu.nextPc);

            // Call the next piece of compiled code.
            callFunc(&cpu);
        }
        std::cout << "xreg[1] is: " << cpu.xreg[1] << '\n';
        std::cout << "nextPc after loop completion is: " << cpu.nextPc << '\n';

        // Emit a little more code.
        jit.EmitAddi(1, 0, 0x3c00); // add x1, x0, 3c00h (15360 - a number burned into my brain).
        jit.EmitAddi(2, 0, 0x3c01); // add x2, x0, 3c01h
        jit.EmitAddi(3, 0, 0x3ff);  // add x3, x0, 3ffh
        jit.EmitEndFunc();

        auto loopEntryOfs2 = jit.EmitAddi(1, 0, 10); // add x1, x0, 10
        jit.EmitAddi(1, 1, -1);                      // add x1, x1, -1
        jit.EmitBne(1, 0, -4);                       // bne x1, x0, -4
        const auto endPc2 = jit.EmitEndFunc();

        // Compile the newly emitted code and call it.
        CpuFunc func3 = jit.Compile();
        func3(&cpu);
        std::cout << "xreg[1] is: " << cpu.xreg[1] << '\n'; // 15360 (0x3c00)
        std::cout << "xreg[2] is: " << cpu.xreg[2] << '\n'; // 15361 (0x3c01)
        std::cout << "xreg[3] is: " << cpu.xreg[3] << '\n'; //  1023 (0x03ff)

        // Call the other loop.
        callFunc = jit.Resolve(loopEntryOfs2);
        callFunc(&cpu);
        std::cout << "xreg[1] is: " << cpu.xreg[1] << '\n';
        while (cpu.nextPc != endPc2)
        {
            // Resolve the address of the next piece of compiled code.
            callFunc = jit.Resolve(cpu.nextPc);

            // Call the next piece of compiled code.
            callFunc(&cpu);
        }
        std::cout << "xreg[1] is: " << cpu.xreg[1] << '\n';
        std::cout << "nextPc after second loop completion is: " << cpu.nextPc << '\n';

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
}
