#include "arviss/arviss.h"                // Address
#include "arviss/platforms/basic/basic.h" // BasicRv32imfCpu
#include "arviss/rv32/rv32.h"             // IsRv32imfCpu

#include <format>
#include <fstream>
#include <iostream>
#include <vector>

using namespace arviss;

template<IsRv32imfCpu T>
auto Run(T& t, size_t count) -> void
{
    while (count > 0 && !t.IsTrapped())
    {
        auto ins = t.Fetch(); // Fetch.
        t.Dispatch(ins);      // Execute.
        --count;
    }
}

auto main(int argc, char* argv[]) -> int
{
    try
    {
        if (argc < 2)
        {
            std::cerr << "Please supply a filename.";
            return 2;
        }

        // Read the image into a buffer.
        const char* filename = argv[1];
        std::ifstream fileHandle(filename, std::ios::in | std::ios::binary | std::ios::ate);
        const std::streampos fileSize = fileHandle.tellg();
        fileHandle.seekg(0, std::ios::beg);
        std::vector<u8> buf(static_cast<size_t>(fileSize));
        fileHandle.read(reinterpret_cast<char*>(buf.data()), fileSize);
        fileHandle.close();

        // Create a CPU.
        basic::BasicRv32imfCpu cpu{};

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
        std::cerr << "Exited with: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
