#include <Windows.h>
#include <iostream>

auto main() -> int
{
    constexpr DWORD flAllocationType = MEM_COMMIT | MEM_RESERVE;
    constexpr DWORD flProtectRW = PAGE_READWRITE;
    constexpr DWORD flProtectRX = PAGE_EXECUTE_READ;

    // Get some zero-initialized memory that we can write to.
    DWORD dwSize = 4096;
    void* mem = VirtualAlloc(nullptr, dwSize, flAllocationType, flProtectRW);
    if (mem == nullptr)
    {
        std::cerr << "VirtualAlloc() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Copy our code into the memory.
    //
    // b8 e8 03 00 00   mov eax, 3e8h ; set the return value to 1000
    // c3               ret           ; return
    char code[] = {'\xb8', '\xe8', '\x03', '\x00', '\x00', '\xc3'};
    memcpy(mem, code, sizeof(code));

    // Make the memory executable.
    DWORD flOldProtect = 0;
    if (FALSE == VirtualProtect(mem, dwSize, flProtectRX, &flOldProtect))
    {
        std::cerr << "VirtualProtect() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Flush the instruction cache.
    if (FALSE == FlushInstructionCache(GetCurrentProcess(), mem, dwSize))
    {
        std::cerr << "FlushInstructionCache() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Call our function. It takes no parameters, and it returns an int.
    using IntFunc = int (*)(void);
    IntFunc myFunc = reinterpret_cast<IntFunc>(mem);
    int result = myFunc();
    std::cout << "Result of calling compiled code is: " << result << '\n';

    // Tidy up.
    constexpr DWORD dwSizeZero = 0;
    if (FALSE == VirtualFree(mem, dwSizeZero, MEM_RELEASE))
    {
        std::cerr << "VirtualFree() failed with " << GetLastError() << '\n';
        return 1;
    }
}
