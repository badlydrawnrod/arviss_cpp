#include <Windows.h>
#include <iostream>

// Approach 1
// ==========
// The idea behind the first approach is to get some memory, write our code to it, then make the memory executable so
// that we can run code from it, as most modern systems will rightly complain if memory is both writable and executable.
//
// Windows API calls used by the first approach:
//
// VirtualAlloc()   - https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc
// VirtualProtect() - https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualprotect
// VirtualFree()    - https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualfree

// Approach 2
// ==========
// The idea behind the second approach is to map the same memory twice; once as writable so that we can write our code
// to it, and once as executable so that we can run our code from it.
//
// Windows API calls used by the second approach:
//
// CreateFileMapping() - https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createfilemappinga
// MapViewOfFile()     - https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile
// UnmapViewOfFile()   - https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-unmapviewoffile

// Rationale
// =========
// The advantage of the second approach for a JIT is that we won't waste memory when writing out small basic blocks. A
// page per basic block seems rather excessive, given that many basic blocks are much smaller than a page. In the
// example code here, the basic block is only 6 bytes.
//
// For a full implementation of these approaches (and more), take a look at AsmJit (https://github.com/asmjit/asmjit/).

auto main() -> int
{
    // --- Approach 1 - Single mapping using VirtualAlloc() / VirtualProtect().

    constexpr DWORD flAllocationType = MEM_COMMIT | MEM_RESERVE;
    constexpr DWORD flProtectRW = PAGE_READWRITE;
    constexpr DWORD flProtectRX = PAGE_EXECUTE_READ;

    // Get some zero-initialized memory that we can write to.
    constexpr DWORD dwSize = 4096;
    void* mem = VirtualAlloc(nullptr, dwSize, flAllocationType, flProtectRW);
    if (mem == nullptr)
    {
        std::cerr << "VirtualAlloc() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Copy our code into memory.
    //
    // b8 e8 03 00 00   mov eax, 3e8h ; set the return value to 1000
    // c3               ret           ; return
    char code[] = {'\xb8', '\xe8', '\x03', '\x00', '\x00', '\xc3'};
    memcpy(mem, code, sizeof(code));

    // Make the memory executable instead of writable.
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

    // --- Approach 2 - Dual mapping using CreateFileMapping() / MapViewOfFile().

    // Create a file mapping, backed by the system paging file.
    constexpr DWORD dwSizeHi = 0;
    constexpr DWORD dwSizeLo = dwSize;
    HANDLE handle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_EXECUTE_READWRITE, dwSizeHi, dwSizeLo, nullptr);
    if (handle == nullptr)
    {
        std::cerr << "CreateFileMapping() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Get a writable view on the memory so that we can write our code to it.
    constexpr DWORD desiredAccessRw = FILE_MAP_READ | FILE_MAP_WRITE;
    void* memRw = MapViewOfFile(handle, desiredAccessRw, 0, 0, static_cast<SIZE_T>(dwSize));
    if (memRw == nullptr)
    {
        std::cerr << "MapViewOfFile() failed when trying to get RW access with " << GetLastError() << '\n';
        return 1;
    }

    // Get an executable view on the *same* memory so that we can execute it.
    constexpr DWORD desiredAccessRx = FILE_MAP_READ | FILE_MAP_EXECUTE;
    void* memRx = MapViewOfFile(handle, desiredAccessRx, 0, 0, static_cast<SIZE_T>(dwSize));
    if (memRx == nullptr)
    {
        std::cerr << "MapViewOfFile() failed when trying to get RX access with " << GetLastError() << '\n';
        return 1;
    }

    // Release the handle as we don't need it now.
    CloseHandle(handle);

    // Copy our code into memory.
    //
    // b8 e9 03 00 00   mov eax, 3e9h ; set the return value to 1001
    // c3               ret           ; return
    char code2[] = {'\xb8', '\xe9', '\x03', '\x00', '\x00', '\xc3'};
    memcpy(memRw, code2, sizeof(code2));

    // Flush the instruction cache.
    if (FALSE == FlushInstructionCache(GetCurrentProcess(), memRx, dwSize))
    {
        std::cerr << "FlushInstructionCache() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Call our code.
    IntFunc myFunc2 = reinterpret_cast<IntFunc>(memRx);
    int result2 = myFunc2();
    std::cout << "Result of calling compiled code is: " << result2 << '\n';

    // Copy the original code into memory.
    memcpy(memRw, code, sizeof(code));

    // Flush the instruction cache.
    if (FALSE == FlushInstructionCache(GetCurrentProcess(), memRx, dwSize))
    {
        std::cerr << "FlushInstructionCache() failed with " << GetLastError() << '\n';
        return 1;
    }

    // Call our code.
    int result3 = myFunc2();
    std::cout << "Result of calling compiled code is: " << result3 << '\n';

    // Unmap the executable view.
    if (FALSE == UnmapViewOfFile(memRx))
    {
        std::cerr << "UnmapViewOfFile() failed when trying to unmap RX access with " << GetLastError() << '\n';
        return 1;
    }

    // Unmap the writable view.
    if (FALSE == UnmapViewOfFile(memRw))
    {
        std::cerr << "UnmapViewOfFile() failed when trying to unmap RW access with " << GetLastError() << '\n';
        return 1;
    }
}
