#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

// Box-drawing characters (Unicode) — must be before hr() which uses BOX_H
constexpr const char* BOX_TL = "┌";
constexpr const char* BOX_TR = "┐";
constexpr const char* BOX_BL = "└";
constexpr const char* BOX_BR = "┘";
constexpr const char* BOX_H  = "─";
constexpr const char* BOX_V  = "│";
constexpr const char* BOX_HD = "┬";
constexpr const char* BOX_HU = "┴";

// Count the visual display width of a UTF-8 string in terminal columns.
// ASCII = 1 column, Cyrillic (2-byte UTF-8) = 1 column.
inline int display_width(const std::string& s) {
    int w = 0;
    for (size_t i = 0; i < s.size(); i++) {
        unsigned char c = static_cast<unsigned char>(s[i]);
        if (c < 0x80) {
            w += 1;
        } else if (c >= 0xC0) {
            w += 1;
            while (i + 1 < s.size() && (static_cast<unsigned char>(s[i+1]) & 0xC0) == 0x80)
                i++;
        }
    }
    return w;
}

// Pad string to target visual width with spaces on the right.
inline std::string pad_right(const std::string& s, int target_width) {
    int w = display_width(s);
    if (w >= target_width) return s;
    return s + std::string(target_width - w, ' ');
}

// Pad string to target visual width with spaces on the left.
inline std::string pad_left(const std::string& s, int target_width) {
    int w = display_width(s);
    if (w >= target_width) return s;
    return std::string(target_width - w, ' ') + s;
}

// Horizontal rule using BOX_H dash character (or pass custom string).
inline std::string hr(int width, const std::string& c = BOX_H) {
    std::string result;
    for (int i = 0; i < width; i++) result += c;
    return result;
}

#endif
