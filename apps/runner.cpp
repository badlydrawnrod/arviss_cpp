#include "arviss/platforms/basic/cpus.h"
#include "arviss/rv32/disassemblers.h"

#include <format>
#include <fstream>
#include <iostream>
#include <vector>

using namespace arviss;

template<IsRv32imfVm T>
auto Run(T& t, size_t count) -> void
{
    while (count > 0 && !t.IsTrapped())
    {
        auto ins = t.Fetch(); // Fetch.
        t.Dispatch(ins);      // Execute.
        --count;
    }
}

// How do I say that U's instruction handler can't be a subset of T's instruction handler?
template<IsRv32imfVm T, IsRv32imfTrace U>
auto Run(T& t, U& u, size_t count) -> void
{
    while (count > 0 && !t.IsTrapped())
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
        std::ifstream fileHandle("examples/images/hello.bin", std::ios::in | std::ios::binary | std::ios::ate);
        const size_t fileSize = fileHandle.tellg();
        fileHandle.seekg(0, std::ios::beg);
        std::vector<u8> buf(fileSize);
        fileHandle.read(reinterpret_cast<char*>(buf.data()), fileSize);
        fileHandle.close();

        // Create a CPU.
        BasicRv32imfCpu cpu{};

        // Populate its memory with the contents of the image.
        Address addr = 0;
        for (auto b : buf)
        {
            cpu.Write8(addr, b);
            ++addr;
        }

        // Execute some instructions.
        cpu.ClearTraps();
        cpu.SetNextPc(0);
        Run(cpu, 10000);

        if (cpu.IsTrapped())
        {
            switch (cpu.TrapCause()->type_)
            {
            case TrapType::Breakpoint:
                std::cerr << "Trapped at breakpoint\n";
                break;
            case TrapType::EnvironmentCallFromMMode:
                std::cerr << "ecall from M mode\n";
                break;
            default:
                std::cerr << "trapped\n";
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "VM exited with: " << e.what() << '\n';
    }
}
