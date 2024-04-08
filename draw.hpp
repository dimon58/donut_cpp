#pragma once

#include "terminal.hpp"

#include <utility>
#include <windows.h>
#include <iostream>


class ConsoleDrawer {
public:
    ConsoleDrawer(short width, short height) {
        _console = GetStdHandle(STD_OUTPUT_HANDLE);

        CONSOLE_FONT_INFO font_info;
        GetCurrentConsoleFont(_console, false, &font_info);
        _font_width = font_info.dwFontSize.X;
        _font_height = font_info.dwFontSize.Y;


        _width = width;
        _height = ScaleY(height);
        _area = _width * _height;
        SetConsoleSize(_width, _height);


        _screen_buf = new char[_area + 1];
        _screen_buf[_area] = '\0';
        _z_buf = new double[_area];

        reset();

    };

    std::pair<int, int> GetFontSize() {
        return std::make_pair(_font_width, _font_height);
    }

    void print_terminal_info() {
        auto s = GetConsoleSize();
        std::cout << "Terminal size " << s.first << "x" << s.second << std::endl;

        auto font_info = GetFontSize();
        std::cout << "Font size " << font_info.first << "x" << font_info.second;
    }

    void flush() {
        DWORD dwBytesWritten = 0;
        WriteConsoleOutputCharacter(
                _console,
                _screen_buf,
                _area,
                {0, 0},
                &dwBytesWritten
        );
    }

    void draw_z() {
        char *buf = new char[_area + 1];
        buf[_area] = '\0';
        for (int i = 0; i < _area; ++i) {
            buf[i] = 'a' + static_cast<int>(_z_buf[i]);
        }
        DWORD dwBytesWritten = 0;
        WriteConsoleOutputCharacter(
                _console,
                buf,
                _area,
                {0, 0},
                &dwBytesWritten
        );
        free(buf);
    }

    inline void fill(char symbol) {
        memset(_screen_buf, symbol, _area * sizeof(char));
    }

    void reset_depth() {
        memset(_z_buf, -1e6, _area * sizeof(double));
    }

    void reset() {
        fill(' ');
        reset_depth();
    }

    [[nodiscard]] inline SHORT ScaleY(SHORT y) const {
        return static_cast<SHORT>( static_cast<int>(y) * _font_width / _font_height);
    }

    [[nodiscard]] inline bool not_in_screen(SHORT x, SHORT y) const {
        return x < 0 || y < 0 || x >= _width || y >= _height;
    }

    void WriteCharToXYNoRescale(SHORT x, SHORT y, char c) {
        if (not_in_screen(x, y)) return;
        _screen_buf[x + (_height - y) * _width] = c;
    }

    void WriteCharToXY(SHORT x, SHORT y, char c) {
        return WriteCharToXYNoRescale(x, ScaleY(y), c);
    }

    void WriteCharToXYZNoRescale(SHORT x, SHORT y, double z, char c) {
        if (not_in_screen(x, y)) return;
        int pos = x + (_height - y) * _width;
        if (z >= _z_buf[pos]) {
            _screen_buf[pos] = c;
            _z_buf[pos] = z;
        }
    }

    void WriteCharToXYZ(SHORT x, SHORT y, double z, char c) {
        return WriteCharToXYZNoRescale(x, ScaleY(y), z, c);
    }

private:
    short _width, _height;
    int _area;
    int _font_width, _font_height;
    HANDLE _console;
    char *_screen_buf;
    double *_z_buf;
};

