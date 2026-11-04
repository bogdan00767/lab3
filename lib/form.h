#ifndef FORM_H
#define FORM_H

#include <string>

/// @brief raw or normalized user data
struct Person {
    std::string surname;
    std::string name;
    std::string patronymic;
    std::string birthDate;
    std::string phone;
    std::string email;
};

/// @brief fix one name: keep only letters and hyphens, first letter upper, rest lower
/// @param s raw name
/// @return fixed name (may be empty)
std::string fixName(const std::string& s);

/// @brief validate and normalize a Person
/// @param raw input data
/// @param out normalized data (filled only if all fields ok)
/// @param error error message if not valid
/// @return true if all fields valid
bool tryNormalizePerson(const Person& raw, Person& out, std::string& error);

/// @brief generate random Person with messy data for tests
/// @return random raw Person
Person generateRandomInput();

#endif