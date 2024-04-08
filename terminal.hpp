#pragma once

#include <cstdint>
#include <utility>
#include <Windows.h>


#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN

#include <Windows.h>

#elif defined(__linux__)
#include <sys/ioctl.h>
#endif // Windows/Linux

/// https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns
std::pair<short, short> GetConsoleSize() {
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return std::make_pair(
            (csbi.srWindow.Right - csbi.srWindow.Left + 1),
            (csbi.srWindow.Bottom - csbi.srWindow.Top + 1)
    );
#elif defined(__linux__)
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    return std::make_pair(w.ws_col, w.ws_rows);
#endif // Windows/Linux
}

/// https://stackoverflow.com/questions/25912721/set-_console-window-size-on-windows
BOOL SetConsoleSize(short cols, short rows) {
#if defined(_WIN32)
    HWND hWnd;
    HANDLE hConOut;
    CONSOLE_FONT_INFO fi;
    CONSOLE_SCREEN_BUFFER_INFO bi;
    int w, h, bw, bh;
    RECT rect = {0, 0, 0, 0};
    COORD coord = {0, 0};
    hWnd = GetConsoleWindow();
    if (hWnd) {
        hConOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConOut && hConOut != (HANDLE) -1) {
            if (GetCurrentConsoleFont(hConOut, FALSE, &fi)) {
                if (GetClientRect(hWnd, &rect)) {
                    w = rect.right - rect.left;
                    h = rect.bottom - rect.top;
                    if (GetWindowRect(hWnd, &rect)) {
                        bw = rect.right - rect.left - w;
                        bh = rect.bottom - rect.top - h;
                        if (GetConsoleScreenBufferInfo(hConOut, &bi)) {
                            coord.X = bi.dwSize.X;
                            coord.Y = bi.dwSize.Y;
                            if (coord.X < cols || coord.Y < rows) {
                                if (coord.X < cols) {
                                    coord.X = cols;
                                }
                                if (coord.Y < rows) {
                                    coord.Y = rows;
                                }
                                if (!SetConsoleScreenBufferSize(hConOut, coord)) {
                                    return FALSE;
                                }
                            }
                            return SetWindowPos(hWnd, nullptr, rect.left, rect.top, cols * fi.dwFontSize.X + bw,
                                                rows * fi.dwFontSize.Y + bh,
                                                SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER);
                        }
                    }
                }
            }
        }
    }
    return FALSE;
#elif defined(__linux__)
#endif // Windows/Linux
}