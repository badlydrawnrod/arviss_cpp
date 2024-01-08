#include <asmjit/asmjit.h>
#include <asmjit/core.h>
#include <iostream>

// Approach 3
// ==========
// Use AsmJit. It's a perfectly good wheel, so why reinvent?
//
// This file is just a rewording of the minimal example from: https://asmjit.com/

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
    a.mov(asmjit::x86::eax, 0x3e8); // mov eax, 3e8h ; set the return value to 1000
    a.ret();                        // ret           ; return

    // The assembler is no longer needed from here, so detach it from the code holder.
    code.detach(&a);

    // Add the generated code to the JitRuntime via JitRuntime::add(). This function copies the code from the code
    // holder into executable memory and relocates it.
    using IntFunc = int (*)(void);
    IntFunc myFunc;
    if (asmjit::Error err = rt.add(&myFunc, &code))
    {
        std::cerr << "AsmJit failed: " << asmjit::DebugUtils::errorAsString(err) << '\n';
        return 1;
    }

    // The code holder is no longer needed and can be safely destroyed. The runtime holds the relocated function and
    // controls its lifetime. The function will be freed with the runtime.
    code.reset();

    // Call our code.
    int result = myFunc();
    std::cout << "Result of calling compiled code is: " << result << '\n';

    // All classes use RAII, all resources will be released before `main()` returns. The generated function can be,
    // however, be released explicitly if you intend to re-use the runtime.
    rt.release(myFunc);

    return 0;
}
