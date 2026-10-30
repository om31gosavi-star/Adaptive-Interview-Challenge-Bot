#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <sstream>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

namespace MockInterview {
namespace Terminal {

// ANSI escape codes
namespace Color {
    inline const std::string RESET       = "\033[0m";
    inline const std::string BOLD        = "\033[1m";
    inline const std::string DIM         = "\033[2m";
    inline const std::string ITALIC      = "\033[3m";
    inline const std::string UNDERLINE   = "\033[4m";

    // Standard foreground
    inline const std::string BLACK       = "\033[30m";
    inline const std::string RED         = "\033[31m";
    inline const std::string GREEN       = "\033[32m";
    inline const std::string YELLOW      = "\033[33m";
    inline const std::string BLUE        = "\033[34m";
    inline const std::string MAGENTA     = "\033[35m";
    inline const std::string CYAN        = "\033[36m";
    inline const std::string WHITE       = "\033[37m";

    // Bright foreground
    inline const std::string BRIGHT_BLACK   = "\033[90m";
    inline const std::string BRIGHT_RED     = "\033[91m";
    inline const std::string BRIGHT_GREEN   = "\033[92m";
    inline const std::string BRIGHT_YELLOW  = "\033[93m";
    inline const std::string BRIGHT_BLUE    = "\033[94m";
    inline const std::string BRIGHT_MAGENTA = "\033[95m";
    inline const std::string BRIGHT_CYAN    = "\033[96m";
    inline const std::string BRIGHT_WHITE   = "\033[97m";

    // Backgrounds
    inline const std::string BG_BLUE        = "\033[44m";
    inline const std::string BG_MAGENTA     = "\033[45m";
    inline const std::string BG_DARK_GRAY   = "\033[100m";
    inline const std::string BG_GREEN       = "\033[42m";
    inline const std::string BG_RED         = "\033[41m";
}

inline void initConsole() {
#ifdef _WIN32
    // Set UTF-8 code pages
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Enable Virtual Terminal Processing for ANSI colors and UTF-8 escape sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif
}

inline void clearScreen() {
    std::cout << "\033[2J\033[1;1H";
}

inline void printBanner() {
    std::cout << Color::BRIGHT_CYAN << Color::BOLD;
    std::cout << R"(
  █████╗ ██████╗  █████╗ ██████╗ ████████╗██╗██╗   ██╗███████╗
 ██╔══██╗██╔══██╗██╔══██╗██╔══██╗╚══██╔══╝██║██║   ██║██╔════╝
 ███████║██║  ██║███████║██████╔╝   ██║   ██║██║   ██║█████╗  
 ██╔══██║██║  ██║██╔══██║██╔═══╝    ██║   ██║╚██╗ ██╔╝██╔══╝  
 ██║  ██║██████╔╝██║  ██║██║        ██║   ██║ ╚████╔╝ ███████╗
 ╚═╝  ╚═╝╚═════╝ ╚═╝  ╚═╝╚═╝        ╚═╝   ╚═╝  ╚═══╝  ╚══════╝
    )" << Color::RESET;

    std::cout << Color::BRIGHT_MAGENTA << Color::BOLD
              << "   >>> AI MOCK INTERVIEW SIMULATION BOT (C++ ENGINE) <<<\n"
              << Color::RESET;
    std::cout << Color::DIM
              << "   Dynamic Difficulty Scaling | Elo System | Multi-Persona | Adaptive Probing\n"
              << Color::RESET;
    std::cout << Color::BRIGHT_BLACK
              << "   ------------------------------------------------------------------------\n\n"
              << Color::RESET;
}

inline void printDivider(const std::string& label = "") {
    std::cout << Color::BRIGHT_BLACK << "───";
    if (!label.empty()) {
        std::cout << " " << Color::BRIGHT_YELLOW << Color::BOLD << label << " " << Color::RESET << Color::BRIGHT_BLACK;
        int remaining = 70 - static_cast<int>(label.length()) - 5;
        if (remaining < 5) remaining = 5;
        for (int i = 0; i < remaining; ++i) std::cout << "─";
    } else {
        for (int i = 0; i < 68; ++i) std::cout << "─";
    }
    std::cout << Color::RESET << "\n";
}

inline void printBadge(const std::string& text, const std::string& colorCode) {
    std::cout << colorCode << Color::BOLD << " [" << text << "] " << Color::RESET;
}

inline void typeWriter(const std::string& text, int charDelayMs = 6, bool enabled = true) {
    if (!enabled || charDelayMs <= 0) {
        std::cout << text << std::flush;
        return;
    }
    for (char c : text) {
        std::cout << c << std::flush;
        if (c == '\n') {
            std::this_thread::sleep_for(std::chrono::milliseconds(charDelayMs * 3));
        } else if (c == '.' || c == '!' || c == '?') {
            std::this_thread::sleep_for(std::chrono::milliseconds(charDelayMs * 4));
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(charDelayMs));
        }
    }
}

inline std::string progressBar(int current, int total, int width = 24) {
    if (total <= 0) total = 1;
    if (current < 0) current = 0;
    if (current > total) current = total;
    double ratio = static_cast<double>(current) / total;
    int filled = static_cast<int>(ratio * width);

    std::ostringstream ss;
    ss << Color::BRIGHT_CYAN << "[";
    for (int i = 0; i < width; ++i) {
        if (i < filled) {
            ss << Color::BRIGHT_GREEN << "█";
        } else {
            ss << Color::BRIGHT_BLACK << "░";
        }
    }
    ss << Color::BRIGHT_CYAN << "] " << Color::BOLD << Color::BRIGHT_WHITE
       << static_cast<int>(ratio * 100.0) << "%" << Color::RESET;
    return ss.str();
}

inline void printBox(const std::vector<std::string>& lines, const std::string& borderColor = Color::BRIGHT_BLUE) {
    size_t maxLen = 0;
    for (const auto& line : lines) {
        if (line.length() > maxLen) maxLen = line.length();
    }
    if (maxLen < 40) maxLen = 40;

    std::cout << borderColor << "┌";
    for (size_t i = 0; i < maxLen + 2; ++i) std::cout << "─";
    std::cout << "┐\n" << Color::RESET;

    for (const auto& line : lines) {
        std::cout << borderColor << "│ " << Color::RESET << line;
        size_t pad = maxLen - line.length();
        for (size_t i = 0; i < pad; ++i) std::cout << " ";
        std::cout << borderColor << " │\n" << Color::RESET;
    }

    std::cout << borderColor << "└";
    for (size_t i = 0; i < maxLen + 2; ++i) std::cout << "─";
    std::cout << "┘\n" << Color::RESET;
}

inline std::string scoreStars(double score) {
    std::ostringstream ss;
    int stars = static_cast<int>((score / 100.0) * 5.0 + 0.5);
    if (stars > 5) stars = 5;
    if (stars < 0) stars = 0;
    ss << Color::BRIGHT_YELLOW;
    for (int i = 0; i < 5; ++i) {
        if (i < stars) ss << "★ ";
        else ss << Color::DIM << "☆ " << Color::BRIGHT_YELLOW;
    }
    ss << Color::RESET;
    return ss.str();
}

} // namespace Terminal
} // namespace MockInterview
