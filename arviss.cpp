#include "arviss.h"

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
