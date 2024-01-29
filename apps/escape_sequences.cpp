// Started life here: https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
//
// System headers
#include <windows.h>

// Standard library C-style
#include <array>
#include <format>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <wchar.h>

#define ESC "\x1b"
#define CSI "\x1b["

bool EnableVTMode()
{
    // TODO: Is this even necessary any more?
    // Set output mode to handle virtual terminal sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
    {
        return false;
    }

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, dwMode))
    {
        return false;
    }
    return true;
}

void PrintBorder(COORD topLeft, COORD bottomRight)
{
    printf(CSI "44m"); // Blue background.
    printf(ESC "(0");  // Enter Line drawing mode

    // Move to the top left.
    printf(CSI "%hd;%hdH", topLeft.Y, topLeft.X);
    printf("\x6c");
    for (int x = topLeft.X + 1; x < bottomRight.X; x++)
    {
        printf("q");
    }
    printf("\x6b");

    // Move to the bottom left.
    printf(CSI "%hd;%hdH", bottomRight.Y, topLeft.X);
    printf("\x6d");
    for (int x = topLeft.X + 1; x < bottomRight.X; x++)
    {
        printf("q");
    }
    printf("\x6a");

    // Draw the left and right borders.
    for (int y = topLeft.Y + 1; y < bottomRight.Y; y++)
    {
        printf(CSI "%hd;%hdH", y, topLeft.X);
        printf("\x78");

        // Technically filling the line isn't really printing a border, but let's play.
        printf(CSI "0K"); // Erase to end of line.

        printf(CSI "%hd;%hdH", y, bottomRight.X);
        printf("\x78");
    }

    printf(ESC "(B"); // exit line drawing mode
    printf(CSI "0m");
}

std::string BeginAlternativeBuffer() { return CSI "?1049h"; }
std::string EndAlternativeBuffer() { return CSI "?1049l"; }
std::string Cls() { return CSI "2J"; }
std::string SetColour(int colour) { return std::format(CSI "{}m", colour); }
std::string MoveTo(int row, int col) { return std::format(CSI "{};{}H", row, col); }

struct Memory
{
    static constexpr int screenWidth = 16;
    static constexpr int screenHeight = 1;

    std::array<std::byte, 32> rodata;
    std::array<char, screenWidth * screenHeight> screen = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '!'};
};

int wmain()
{
    // First, enable VT mode
    bool fSuccess = EnableVTMode();
    if (!fSuccess)
    {
        printf("Unable to enter VT processing mode. Quitting.\n");
        return -1;
    }
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        printf("Couldn't get the console handle. Quitting.\n");
        return -1;
    }

    CONSOLE_SCREEN_BUFFER_INFO ScreenBufferInfo;
    GetConsoleScreenBufferInfo(hOut, &ScreenBufferInfo);
    COORD Size;
    Size.X = ScreenBufferInfo.srWindow.Right - ScreenBufferInfo.srWindow.Left + 1;
    Size.Y = ScreenBufferInfo.srWindow.Bottom - ScreenBufferInfo.srWindow.Top + 1;

    std::stringstream out;

    out << BeginAlternativeBuffer() << Cls();
    std::cout << out.str();
    out.clear();
    PrintBorder({1, 1}, Size);
    auto wch = _getwch();

    const int height = static_cast<int>(Size.Y);
    const int width = static_cast<int>(Size.X);
    std::vector<Memory> memories(32);
    int colour = 41;
    size_t mindex = 0;
    out << Cls();
    for (int row = 1; mindex < memories.size() && row <= height - (Memory::screenHeight - 1); row += Memory::screenHeight)
    {
        for (int col = 1; mindex < memories.size() && col <= width - (Memory::screenWidth - 1); col += Memory::screenWidth)
        {
            char* data = memories[mindex].screen.data();
            out << SetColour(colour);
            for (int screenLine = 0; screenLine < Memory::screenHeight; screenLine++)
            {
                out << MoveTo(row + screenLine, col);
                for (auto cp = 0; cp < Memory::screenWidth; cp++)
                {
                    char ch = static_cast<char>(*data++);
                    out << ((ch >= ' ' && ch < '\x7f') ? ch : ' ');
                }
            }
            ++mindex;
            if (++colour == 47)
            {
                colour = 41;
            }
        }
    }
    std::cout << out.str();

    // Move the cursor outside the viewport.
    short x = 1000;
    short y = 1000;
    printf(CSI "%hd;%hdH", y, x);

    // Ask where the cursor is. It should give us the position clipped to the viewport.
    printf(CSI "6n");

    // Read the cursor position in the form Esc [ <r> ; <c> R
    wchar_t buf[128] = {};
    size_t i = 0;
    for (auto ch = _getwch(); ch != 'R'; ch = _getwch())
    {
        buf[i] = ch;
        i++;
        buf[i] = L'\0';
    }
    printf(CSI "0m");

    wch = _getwch();

    out << EndAlternativeBuffer();
    std::cout << out.str();
    printf("%ws\n", buf + 2); // rows;cols
}
