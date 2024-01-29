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

void PrintVerticalBorder()
{
    printf(ESC "(0");      // Enter Line drawing mode
    printf(CSI "104;93m"); // bright yellow on bright blue
    printf("x");           // in line drawing mode, \x78 -> \u2502 "Vertical Bar"
    printf(CSI "0m");      // restore color
    printf(ESC "(B");      // exit line drawing mode
}

void PrintHorizontalBorder(COORD const Size, bool fIsTop)
{
    printf(ESC "(0");           // Enter Line drawing mode
    printf(CSI "104;93m");      // Make the border bright yellow on bright blue
    printf(fIsTop ? "l" : "m"); // print left corner

    for (int i = 1; i < Size.X - 1; i++) printf("q"); // in line drawing mode, \x71 -> \u2500 "HORIZONTAL SCAN LINE-5"

    printf(fIsTop ? "k" : "j"); // print right corner
    printf(CSI "0m");
    printf(ESC "(B"); // exit line drawing mode
}

void PrintStatusLine(const char* const pszMessage, COORD const Size)
{
    printf(CSI "%d;1H", Size.Y);
    printf(CSI "K"); // clear the line
    printf(pszMessage);
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

    // Enter the alternate buffer
    printf(CSI "?1049h");

    // Clear screen, tab stops, set, stop at columns 16, 32
    printf(CSI "1;1H");
    printf(CSI "2J"); // Clear screen

    int iNumTabStops = 4; // (0, 20, 40, width)
    printf(CSI "3g");     // clear all tab stops
    printf(CSI "1;20H");  // Move to column 20
    printf(ESC "H");      // set a tab stop

    printf(CSI "1;40H"); // Move to column 40
    printf(ESC "H");     // set a tab stop

    // Set scrolling margins to 3, h-2
    printf(CSI "3;%dr", Size.Y - 2);
    int iNumLines = Size.Y - 4;

    printf(CSI "1;1H");
    printf(CSI "102;30m");
    printf("Windows 10 Anniversary Update - VT Example");
    printf(CSI "0m");

    // Print a top border - Yellow
    printf(CSI "2;1H");
    PrintHorizontalBorder(Size, true);

    // // Print a bottom border
    printf(CSI "%d;1H", Size.Y - 1);
    PrintHorizontalBorder(Size, false);

    wchar_t wch;

    // draw columns
    printf(CSI "3;1H");
    int line = 0;
    for (line = 0; line < iNumLines * iNumTabStops; line++)
    {
        PrintVerticalBorder();
        if (line + 1 != iNumLines * iNumTabStops) // don't advance to next line if this is the last line
            printf("\t");                         // advance to next tab stop
    }

    PrintStatusLine("Press any key to see text printed between tab stops.", Size);
    wch = _getwch();

    // Fill columns with output
    printf(CSI "3;1H");
    for (line = 0; line < iNumLines; line++)
    {
        int tab = 0;
        for (tab = 0; tab < iNumTabStops - 1; tab++)
        {
            PrintVerticalBorder();
            printf("line=%d", line);
            printf("\t"); // advance to next tab stop
        }
        PrintVerticalBorder(); // print border at right side
        if (line + 1 != iNumLines)
            printf("\t"); // advance to next tab stop, (on the next line)
    }

    PrintStatusLine("Press any key to demonstrate scroll margins", Size);
    wch = _getwch();

    printf(CSI "3;1H");
    for (line = 0; line < iNumLines * 2; line++)
    {
        printf(CSI "K"); // clear the line
        int tab = 0;
        for (tab = 0; tab < iNumTabStops - 1; tab++)
        {
            PrintVerticalBorder();
            printf("line=%d", line);
            printf("\t"); // advance to next tab stop
        }
        PrintVerticalBorder(); // print border at right side
        if (line + 1 != iNumLines * 2)
        {
            printf("\n"); // Advance to next line. If we're at the bottom of the margins, the text will scroll.
            printf("\r"); // return to first col in buffer
        }
    }

    PrintStatusLine("Press any key to exit", Size);
    wch = _getwch();

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

    PrintBorder({1, 1}, Size);

    const int height = static_cast<int>(Size.Y);
    const int width = static_cast<int>(Size.X);
    std::vector<Memory> memories(32);
    int colour = 41;
    size_t mindex = 0;
    std::stringstream out;
    for (int row = 1; mindex < memories.size() && row <= height - (Memory::screenHeight - 1); row += Memory::screenHeight)
    {
        for (int col = 1; mindex < memories.size() && col <= width - (Memory::screenWidth - 1); col += Memory::screenWidth)
        {
            char* data = memories[mindex].screen.data();
            out << SetColour(colour);
            for (int screenLine = 0; screenLine < Memory::screenHeight; screenLine++)
            {
                out << MoveTo(row + screenLine, col);
                for (auto cp = 0u; cp < Memory::screenWidth; cp++)
                {
                    char ch = static_cast<char>(*data++);
                    if (ch >= ' ' && ch < '\x7f')
                    {
                        out << ch;
                    }
                    else
                    {
                        out << ' ';
                    }
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

    printf(CSI "0m");

    fprintf(stderr, CSI "?1003h" CSI "?1015h" CSI "?1006h");

    wch = _getwch();

    // Exit the alternate buffer
    printf(CSI "?1049l");

    printf("%ws\n", buf + 1);
    printf("size: (r, c) = (%hd, %hd)\n", Size.Y, Size.X);
}
