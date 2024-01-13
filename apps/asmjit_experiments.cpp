#include <asmjit/asmjit.h>
#include <asmjit/core.h>
#include <cstddef>
#include <format>
#include <iostream>

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

// Add two registers and store the result in a third.
auto EmitAdd(asmjit::x86::Assembler& a, Reg rd, Reg rs1, Reg rs2) -> void
{
    using namespace asmjit;

    const x86::Mem addrRs1 = XregOfs(rs1);
    const x86::Mem addrRs2 = XregOfs(rs2);
    const x86::Mem addrRd = XregOfs(rd);

    // Only emit code if we're not writing to x0, as that's always zero.
    if (rd != 0)
    {
        a.mov(x86::eax, addrRs1);
        a.add(x86::eax, addrRs2);
        a.mov(addrRd, x86::eax);
    }
}

// Add a register and an immediate value and store the result in a destination register.
auto EmitAddi(asmjit::x86::Assembler& a, Reg rd, Reg rs1, int32_t imm) -> void
{
    using namespace asmjit;

    const x86::Mem addrRs1 = XregOfs(rs1);
    const x86::Mem addrRd = XregOfs(rd);

    // Only emit code if we're not writing to rd, as that's always zero.
    if (rd != 0)
    {
        // Add if rs1 is not x0, otherwise just move.
        if (rs1 != 0)
        {
            a.mov(x86::eax, addrRs1);
            a.add(x86::eax, imm);
        }
        else
        {
            a.mov(x86::eax, imm);
        }
        a.mov(addrRd, x86::eax);
    }
}

// Compare two registers and branch if they aren't equal.

auto EmitBne(asmjit::x86::Assembler& a, Reg rs1, Reg rs2, int32_t imm) -> void
{
    using namespace asmjit;

    const x86::Mem addrRs1 = XregOfs(rs1);
    const x86::Mem addrRs2 = XregOfs(rs2);
    Label skip = a.newLabel();

    // TODO: what if it's the same register?
    a.mov(x86::eax, addrRs1); // TODO: what if this was x0 ?
    a.cmp(x86::eax, addrRs2); // TODO: what if this was x0 ?
    a.je(skip);

    a.mov(x86::eax, PcOfs());
    a.add(x86::eax, imm);
    a.mov(NextPcOfs(), x86::eax);

    a.bind(skip);
    a.ret(); // Return to the execution environment.
}

auto main() -> int
{
    // Runtime designed for JIT - it holds relocated functions and controls their lifetime.
    asmjit::JitRuntime rt;

    asmjit::FileLogger logger(stdout);

    // Holds code and relocation information during code generation.
    asmjit::CodeHolder code;

    // Initialize the code holder so that it can be used.
    code.init(rt.environment(), rt.cpuFeatures());
    code.setLogger(&logger);

    // Create an assembler that can emit code to the code holder. The assembler's constructor calls `code.attach(&a)`
    // implicitly.
    asmjit::x86::Assembler a(&code);

    // Use the assembler to emit some code to the code holder's .text section.
    asmjit::Label startLabel = a.newLabel();
    a.bind(startLabel);
    std::cout << std::format("startLabel is at: 0x{:08x}\n", code.labelOffset(startLabel));

    EmitAdd(a, 1, 2, 3);    // add x1, x2, x3
    EmitAdd(a, 0, 1, 1);    // add x0, x1, x1
    EmitAdd(a, 2, 2, 2);    // add x2, x2, x2
    EmitAddi(a, 3, 0, 100); // addi x3, x0, 100
    a.ret();

    // Emit a loop that counts down from 10 to 0.
    asmjit::Label loopFuncEntryLabel = a.newLabel();
    a.bind(loopFuncEntryLabel);
    EmitAddi(a, 1, 0, 10); // add x1, x0, 10

    // TODO: how are we keeping track of the VM's PC?
    asmjit::Label loopLabel = a.newLabel();
    a.bind(loopLabel);
    EmitAddi(a, 1, 1, -1); // add x1, x1, -1
    EmitBne(a, 1, 0, -4);  // bne x1, x0, -4

    // The assembler is no longer needed from here, so detach it from the code holder.
    code.detach(&a);

    // Add the generated code to the JitRuntime via JitRuntime::add(). This function copies the code from the code
    // holder into executable memory and relocates it.
    CpuFunc simpleFunc;
    if (asmjit::Error err = rt.add(&simpleFunc, &code))
    {
        std::cerr << "AsmJit failed: " << asmjit::DebugUtils::errorAsString(err) << '\n';
        return 1;
    }

    std::cout << std::format("loopFuncEntryLabel is at: 0x{:08x}\n", code.labelOffset(loopFuncEntryLabel));
    auto loopFuncEntryAddr = reinterpret_cast<uint8_t*>(&(*simpleFunc)) + code.labelOffset(loopFuncEntryLabel);
    auto loopFuncAddr = reinterpret_cast<CpuFunc>(loopFuncEntryAddr);

    auto loopAddr = reinterpret_cast<uint8_t*>(&(*simpleFunc)) + code.labelOffset(loopLabel);
    auto loopFunc = reinterpret_cast<CpuFunc>(loopAddr);
    std::cout << std::format("loopLabel is at: 0x{:08x}\n", code.labelOffset(loopLabel));

    // The code holder is no longer needed and can be safely destroyed. The runtime holds the relocated function and
    // controls its lifetime. The function will be freed with the runtime.
    code.reset();

    // Call our code.
    Cpu am{};
    am.xreg[1] = 1234;
    am.xreg[2] = 20;
    am.xreg[3] = 22;
    std::cout << "xreg[1] before calling compiled code is: " << am.xreg[1] << '\n';
    std::cout << "xreg[2] before calling compiled code is: " << am.xreg[2] << '\n';
    simpleFunc(&am);
    std::cout << "xreg[1] after calling compiled code is: " << am.xreg[1] << '\n'; // 42   (from add x1, x2, x3)
    std::cout << "xreg[2] after calling compiled code is: " << am.xreg[2] << '\n'; // 40   (from add x2, x2, x2)
    std::cout << "xreg[3] after calling compiled code is: " << am.xreg[3] << '\n'; // 100  (from addi, x3, x0, 100)

    // Run the loop.

    // Now to live dangerously.
    loopFuncAddr(&am);
    while (am.xreg[1] != 0)
    {
        std::cout << "xreg[1] after calling loop code is: " << am.xreg[1] << '\n';
        std::cout << "next_pc after calling loop code is: " << am.nextPc << '\n';

        // TODO: move next_pc to pc, then look up where to go based on pc (should map to a label, and if it doesn't
        //       then we need to JIT some more).
        loopFunc(&am);
    }
    std::cout << "xreg[1] after loop completion is: " << am.xreg[1] << '\n';
    std::cout << "next_pc after loop completion is: " << am.nextPc << '\n';

    // All classes use RAII, all resources will be released before `main()` returns. The generated function can be,
    // however, be released explicitly if you intend to re-use the runtime.
    rt.release(simpleFunc);

    return 0;
}
