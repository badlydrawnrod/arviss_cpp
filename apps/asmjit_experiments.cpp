#include <asmjit/asmjit.h>
#include <asmjit/core.h>
#include <cstddef>
#include <iostream>

// Enough of a CPU for experimental purposes.
struct Cpu
{
    uint32_t xreg[32]; // Integer registers.
};

// How we invoke a function that works on the CPU.
using CpuFunc = void (*)(Cpu*);

using Reg = uint32_t;

// Let's assume WIN32, x64 for now. Calling convention is that the first argument is in rcx.
constexpr asmjit::x86::Gp ARG0 = asmjit::x86::rcx; // First argument.

inline auto xreg_ofs(Reg r) -> asmjit::x86::Mem
{
    // What's the address of the given xregister relative to ARG0.
    return asmjit::x86::ptr(ARG0, static_cast<int32_t>(offsetof(Cpu, xreg[r])));
}

// Add two registers and store the result in a third.
auto EmitAdd(asmjit::x86::Assembler& a, Reg rd, Reg rs1, Reg rs2) -> void
{
    using namespace asmjit;

    const x86::Mem addrRs1 = xreg_ofs(rs1);
    const x86::Mem addrRs2 = xreg_ofs(rs2);
    const x86::Mem addrRd = xreg_ofs(rd);

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

    const x86::Mem addrRs1 = xreg_ofs(rs1);
    const x86::Mem addrRd = xreg_ofs(rd);

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
    EmitAdd(a, 1, 2, 3);    // add x1, x2, x3
    EmitAdd(a, 0, 1, 1);    // add x0, x1, x1
    EmitAdd(a, 2, 2, 2);    // add x2, x2, x2
    EmitAddi(a, 3, 0, 100); // addi x3, x0, 100
    a.ret();

    // The assembler is no longer needed from here, so detach it from the code holder.
    code.detach(&a);

    // Add the generated code to the JitRuntime via JitRuntime::add(). This function copies the code from the code
    // holder into executable memory and relocates it.
    CpuFunc myFunc;
    if (asmjit::Error err = rt.add(&myFunc, &code))
    {
        std::cerr << "AsmJit failed: " << asmjit::DebugUtils::errorAsString(err) << '\n';
        return 1;
    }

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
    myFunc(&am);
    std::cout << "xreg[1] after calling compiled code is: " << am.xreg[1] << '\n'; // 42   (from add x1, x2, x3)
    std::cout << "xreg[2] after calling compiled code is: " << am.xreg[2] << '\n'; // 40   (from add x2, x2, x2)
    std::cout << "xreg[3] after calling compiled code is: " << am.xreg[3] << '\n'; // 100  (from addi, x3, x0, 100)

    // All classes use RAII, all resources will be released before `main()` returns. The generated function can be,
    // however, be released explicitly if you intend to re-use the runtime.
    rt.release(myFunc);

    return 0;
}
