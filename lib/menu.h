#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <stdexcept>
#include "json.hpp"

/// @brief menu and translations from locales.json
class Menu {
private:
    /// @brief whole locales.json
    nlohmann::json data;

    /// @brief current language section (ru/en/de)
    nlohmann::json current;

    /// @brief selected language code
    std::string lang;

    /// @brief read a number, repeat until it is in range
    /// @param prompt text before input
    /// @param minV lowest allowed value
    /// @param maxV highest allowed value
    /// @return valid number
    int readInt(const std::string& prompt, int minV, int maxV);

public:
    /// @brief load locales.json
    /// @param jsonPath path to json file
    /// @throws std::runtime_error if file cannot be opened
    Menu(const std::string& jsonPath);

    /// @brief ask user to pick a language
    /// @return 1 = ru, 2 = en, 3 = de
    int chooseLanguage();

    /// @brief show task menu, read choice
    /// @return 0 = exit, 1..3 = task number
    int chooseTask();

    /// @brief public wrapper for readInt
    /// @param prompt text before input
    /// @param minV lowest allowed value
    /// @param maxV highest allowed value
    /// @return valid number
    int ask(const std::string& prompt, int minV, int maxV) { return readInt(prompt, minV, maxV); }
    
    /// @brief translate key to current language
    /// @param key key from locales.json
    /// @return text in current language (or key if not found)
    std::string tr(const std::string& key) const;

    /// @brief current language code
    /// @return "ru", "en" or "de"
    std::string langCode() const { return lang; }
};

#endif