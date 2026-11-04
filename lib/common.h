#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <locale>
#include <cwctype>
#include <cstdlib>
#include <cstring>
#include <cerrno>

/// @brief set console locale to system default (UTF-8 on macOS)
inline void initConsole() {
    std::setlocale(LC_ALL, "");
}

/// @brief convert UTF-8 string to wide string (wchar_t) via current locale
/// @param s UTF-8 string
/// @return wide string, empty on error
inline std::wstring utf8_to_ws(const std::string& s) {
    if (s.empty()) return L"";

    std::mbstate_t state{};
    const char* src = s.c_str();
    size_t len = std::mbsrtowcs(nullptr, &src, 0, &state);
    if (len == static_cast<size_t>(-1)) return L"";

    std::wstring out(len, L'\0');
    src = s.c_str();
    state = std::mbstate_t{};
    std::mbsrtowcs(&out[0], &src, len, &state);
    return out;
}

/// @brief convert wide string (wchar_t) back to UTF-8 via current locale
/// @param s wide string
/// @return UTF-8 string, empty on error
inline std::string ws_to_utf8(const std::wstring& s) {
    if (s.empty()) return "";

    std::mbstate_t state{};
    const wchar_t* src = s.c_str();
    size_t len = std::wcsrtombs(nullptr, &src, 0, &state);
    if (len == static_cast<size_t>(-1)) return "";

    std::string out(len, '\0');
    src = s.c_str();
    state = std::mbstate_t{};
    std::wcsrtombs(&out[0], &src, len, &state);
    return out;
}

/// @brief trim spaces, tabs and newlines at both ends
/// @param s input string
/// @return trimmed string
inline std::string trim(const std::string& s) {
    const size_t b = s.find_first_not_of(" \t\r\n");
    if (b == std::string::npos) return "";
    const size_t e = s.find_last_not_of(" \t\r\n");
    return s.substr(b, e - b + 1);
}

#endif