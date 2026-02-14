#pragma once
#include <cstdlib>
#include <format>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace tty {

inline bool terminalSupportsColor() {
    static bool supported = [] {
        if (std::getenv("NO_COLOR")) return false;

#if defined(_WIN32)
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return false;

        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) return false;

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        if (!SetConsoleMode(hOut, mode)) return false;

        return true;
#else
        const char *term = std::getenv("TERM");
        if (!term) return false;
        std::string_view t{term};
        return t != "dumb";
#endif
    }();
    return supported;
}

namespace ansi {
constexpr std::string_view esc = "\033[";
constexpr std::string_view end = "m";
}  // namespace ansi

enum class Color {
    Default = 39,
    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    BrightBlack = 90,
    BrightRed,
    BrightGreen,
    BrightYellow,
    BrightBlue,
    BrightMagenta,
    BrightCyan,
    BrightWhite
};

enum class Style {
    Reset = 0,
    Bold = 1,
    Dim = 2,
    Italic = 3,
    Underline = 4,
    Blink = 5,
    Reverse = 7,
    Hidden = 8,
    Strike = 9
};

inline std::string code(int n) {
    if (!terminalSupportsColor()) return {};
    return std::format("{}{}{}", ansi::esc, n, ansi::end);
}

inline std::string fg(Color c) { return code(static_cast<int>(c)); }
inline std::string style(Style s) { return code(static_cast<int>(s)); }
inline std::string reset() { return code(0); }

template <typename... Args>
inline void log(std::format_string<Args...> fmt, Args &&...args) {
    std::cout << fg(Color::Blue) << "[BASALT | LOG] " << reset()
              << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

template <typename... Args>
inline void warn(std::format_string<Args...> fmt, Args &&...args) {
    std::cout << fg(Color::Yellow) << "[BASALT | WRN] " << reset()
              << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

template <typename... Args>
inline void err(std::format_string<Args...> fmt, Args &&...args) {
    std::cout << fg(Color::Red) << "[BASALT | ERR] " << reset()
              << std::format(fmt, std::forward<Args>(args)...) << '\n';
}

}  // namespace tty
