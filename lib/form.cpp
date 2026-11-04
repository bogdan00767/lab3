#include "form.h"
#include "common.h"
#include <regex>
#include <algorithm>
#include <cstdlib>
#include <cstdio>
#include <cwctype>
#include <fstream>
#include <vector>
#include <stdexcept>


std::string fixName(const std::string& s) {
    if (s.empty()) return s;

    std::wstring w = utf8_to_ws(s);
    std::wstring out;
    bool upperNext = true;

    for (wchar_t c : w) {
        if (c == 0) continue;

        bool isLetter = std::iswalpha(c) != 0;
        bool isHyphen = (c == L'-');

        if (isLetter || isHyphen) {
            if (isHyphen) {
                out += c;
                upperNext = true;
            } else {
                out += upperNext ? std::towupper(c) : std::towlower(c);
                upperNext = false;
            }
        }
    }
    return ws_to_utf8(out);
}

bool tryNormalizePerson(const Person& raw, Person& out, std::string& error) {
    std::string errMsg = "";

    out.surname    = fixName(raw.surname);
    out.name       = fixName(raw.name);
    out.patronymic = fixName(raw.patronymic);
    if (out.surname.empty() || out.name.empty()) errMsg += "Пустое ФИО. ";

    std::string dateStr = trim(raw.birthDate);
    static const std::regex reYMD(R"(^(\d{4})[-./](\d{1,2})[-./](\d{1,2})$)");
    static const std::regex reDMY(R"(^(\d{1,2})[-./](\d{1,2})[-./](\d{4})$)");
    std::smatch m;

    int year = 0, month = 0, day = 0;
    bool dateOk = false;

    if (std::regex_match(dateStr, m, reYMD)) {
        year  = std::stoi(m[1].str());
        month = std::stoi(m[2].str());
        day   = std::stoi(m[3].str());
        dateOk = true;
    } else if (std::regex_match(dateStr, m, reDMY)) {
        day   = std::stoi(m[1].str());
        month = std::stoi(m[2].str());
        year  = std::stoi(m[3].str());
        dateOk = true;
    }

    if (!dateOk || month < 1 || month > 12 || day < 1 || day > 31) {
        errMsg += "Неверная дата. ";
    } else {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02d.%02d.%04d", day, month, year);
        out.birthDate = buf;
    }

    std::string digits;
    for (char c : raw.phone) if (isdigit((unsigned char)c)) digits += c;
    if (digits.size() == 11 && digits[0] == '8') digits[0] = '7';
    else if (digits.size() == 10) digits = "7" + digits;
    if (digits.size() != 11) {
        errMsg += "Неверный телефон. ";
    } else {
        out.phone = "+" + digits;
    }

    std::string em = trim(raw.email);
    if (!em.empty() && em.front() == '<') em = em.substr(1, em.size() - 2);
    std::transform(em.begin(), em.end(), em.begin(), ::tolower);

    static const std::regex reEmail(R"(^[a-z0-9._%+-]+@[a-z0-9.-]+\.[a-z]{2,}$)");
    if (!std::regex_match(em, reEmail)) {
        errMsg += "Неверный email. ";
    } else {
        out.email = em;
    }

    if (!errMsg.empty()) {
        error = errMsg;
        return false;
    }
    return true;
}

/// @brief load one dictionary file (one word per line, empty lines skipped)
/// @param path path to txt file
/// @return vector of words
/// @throws std::runtime_error if file cannot be opened
static std::vector<std::string> loadDict(const std::string& path) {
    std::vector<std::string> v;
    std::ifstream f(path);
    if (!f.is_open()) {
        throw std::runtime_error("error: dict wasnt open " + path);
    }
    std::string line;
    while (std::getline(f, line)) {
        line = trim(line);
        if (!line.empty()) v.push_back(line);
    }
    return v;
}

/// @brief random int in [lo, hi]
/// @param lo low bound
/// @param hi high bound
/// @return random number
static int randInt(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}

/// @brief make a name "dirty": random upper/lower case letters
/// @param s clean name
/// @return name with random case
static std::string messyCase(const std::string& s) {
    std::wstring w = utf8_to_ws(s);
    for (auto& c : w) {
        if (!std::iswalpha(c)) continue;
        if (rand() % 2 != 0) continue;

        c = std::iswupper(c) ? std::towlower(c) : std::towupper(c);
    }
    return ws_to_utf8(w);
}


Person generateRandomInput() {
    static std::vector<std::string> surnames =
        loadDict("data/surnames.txt");
    static std::vector<std::string> names =
        loadDict("data/names.txt");
    static std::vector<std::string> patronymics =
        loadDict("data/patronymics.txt");

    if (surnames.empty() || names.empty() || patronymics.empty()) {
        throw std::runtime_error("error: dict is empty");
    }

    Person p;
    p.surname    = messyCase(surnames[rand() % surnames.size()]);
    p.name       = messyCase(names[rand() % names.size()]);
    p.patronymic = messyCase(patronymics[rand() % patronymics.size()]);

    // sometimes add a digit into patronymic (like the methodichka example "ПетРОви1ч")
    if (rand() % 3 == 0 && !p.patronymic.empty()) {
        std::wstring w = utf8_to_ws(p.patronymic);
        if (w.size() > 2) {
            w.insert(w.begin() + 2, L'1');
            p.patronymic = ws_to_utf8(w);
        }
    }

    // date: one of 4 formats (all accepted by tryNormalizePerson)
    int year  = randInt(1980, 2005);
    int month = randInt(1, 12);
    int day   = randInt(1, 28);
    char buf[64];
    switch (rand() % 4) {
        case 0: snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day); break;  // 2002-03-01
        case 1: snprintf(buf, sizeof(buf), "%02d.%02d.%04d", day, month, year); break;  // 01.03.2002
        case 2: snprintf(buf, sizeof(buf), "%02d/%02d/%04d", day, month, year); break;  // 01/03/2002
        default: snprintf(buf, sizeof(buf), "%04d/%02d/%02d", year, month, day); break; // 2002/03/01
    }
    p.birthDate = buf;

    // phone: one of 4 messy formats (all accepted by tryNormalizePerson)
    int op = randInt(900, 999);
    int a  = randInt(100, 999);
    int b  = randInt(10, 99);
    int c  = randInt(10, 99);
    switch (rand() % 4) {
        case 0: snprintf(buf, sizeof(buf), "+7%d%03d%02d%02d", op, a, b, c); break;
        case 1: snprintf(buf, sizeof(buf), "8(%d)%03d-%02d-%02d", op, a, b, c); break;
        case 2: snprintf(buf, sizeof(buf), "8 %d %03d %02d %02d", op, a, b, c); break;
        default: snprintf(buf, sizeof(buf), "+7-%d-%03d-%02d-%02d", op, a, b, c); break;
    }
    p.phone = buf;

    // email: random local, sometimes in <> and/or upper case
    snprintf(buf, sizeof(buf), "User%d@Example.COM", randInt(100, 999));
    p.email = buf;
    if (rand() % 2 == 0) p.email = "<" + p.email + ">";

    return p;
}