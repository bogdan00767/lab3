#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <vector>
#include <exception>
#include <cstdlib>
#include <ctime>

#include "common.h"
#include "menu.h"
#include "asciidoc.h"
#include "form.h"

using namespace std;

/// @brief print one person in one line
/// @param p person to print
static void printPerson(const Person& p) {
    cout << p.surname << " " << p.name << " " << p.patronymic << " "
         << p.birthDate << " " << p.phone << " " << p.email << "\n";
}

/// @brief read one line from console
/// @return line without trailing newline
static string readLine() {
    string s;
    getline(cin, s);
    return s;
}

/// @brief read all form fields from console
/// @param menu menu for translations
/// @return raw person
static Person inputPerson(Menu& menu) {
    Person p;
    cout << menu.tr("prompt_surname");    p.surname    = readLine();
    cout << menu.tr("prompt_name");       p.name       = readLine();
    cout << menu.tr("prompt_patronymic"); p.patronymic = readLine();
    cout << menu.tr("prompt_date");       p.birthDate  = readLine();
    cout << menu.tr("prompt_phone");      p.phone      = readLine();
    cout << menu.tr("prompt_email");      p.email      = readLine();
    return p;
}

/// @brief normalize one person and print result
/// @param raw raw person
/// @param menu menu for translations
static void processOne(const Person& raw, Menu& menu) {
    Person norm;
    string error;
    if (tryNormalizePerson(raw, norm, error)) {
        cout << menu.tr("form_ok") << " ";
        printPerson(norm);
    } else {
        cout << menu.tr("form_err") << " " << error << "\n";
    }
}

/// @brief task 1: check file is valid AsciiDoc
/// @param menu menu for translations
static void runCheck(Menu& menu) {
    cout << menu.tr("enter_filename");
    string filename = readLine();

    try {
        FileResult r = analyzeAsciiDocFile(filename);

        if (r.valid) {
            cout << menu.tr("check_ok") << "\n";
        } else {
            cout << menu.tr("check_err") << ": " << r.error << "\n";
        }

        cout << menu.tr("stat_lines") << ": " << r.lines << "\n";
    } catch (const exception& e) {
        cerr << menu.tr("err_file_open") << ": " << e.what() << "\n";
    }
    cout << "\n";
}

/// @brief task 2: find elements, print only counts (skip invalid files)
/// @param menu menu for translations
static void runExtract(Menu& menu) {
    cout << menu.tr("enter_filename");
    string filename = readLine();

    try {
        FileResult r = analyzeAsciiDocFile(filename);

        // if not valid asciidoc - report and stop
        if (!r.valid) {
            cout << menu.tr("check_err") << ": " << r.error << "\n\n";
            return;
        }

        cout << "\n" << menu.tr("stats_title") << ":\n";
        cout << "  " << menu.tr("stat_headings")   << ": " << r.stats.headings   << "\n";
        cout << "  " << menu.tr("stat_paragraphs") << ": " << r.stats.paragraphs << "\n";
        cout << "  " << menu.tr("stat_lists")      << ": " << r.stats.lists      << "\n";
    } catch (const exception& e) {
        cerr << menu.tr("err_file_open") << ": " << e.what() << "\n";
    }
    cout << "\n";
}

/// @brief task 3: form (manual / random / example)
/// @param menu menu for translations
static void runForm(Menu& menu) {
    while (true) {
        cout << "\n=== " << menu.tr("form_title") << " ===\n";
        cout << "1. " << menu.tr("test_manual")  << "\n";
        cout << "2. " << menu.tr("test_random")  << "\n";
        cout << "3. " << menu.tr("test_example") << "\n";
        cout << "0. " << menu.tr("form_back")    << "\n";

        int choice = menu.ask(menu.tr("prompt"), 0, 3);
        if (choice == 0) return;

        try {
            if (choice == 1) {
                cout << "\n";
                Person userRaw = inputPerson(menu);
                cout << "\n";
                processOne(userRaw, menu);
                cout << "\n";
            }
            else if (choice == 2) {
                cout << "\n";
                for (int i = 0; i < 5; i++) {
                    Person raw = generateRandomInput();
                    cout << menu.tr("form_was");    printPerson(raw);
                    cout << menu.tr("form_became"); processOne(raw, menu);
                    cout << "\n";
                }
            }
            else if (choice == 3) {
                cout << "\n";
                Person raw = { "Сидоров", "ИваН", "ПетРОви1ч", "2002-03-01", "+7996-8432-431", "<sidorovVV@pochta.ru>" };
                cout << menu.tr("form_was");    printPerson(raw);
                cout << menu.tr("form_became"); processOne(raw, menu);
                cout << "\n";
            }
        } catch (const exception& e) {
            cerr << menu.tr("error_generic") << ": " << e.what() << "\n\n";
        }
    }
}

int main() {
    initConsole();
    srand((unsigned)time(nullptr));

    try {
        Menu menu("/Users/bogdanvinogradov/Desktop/5sem/ptppovs/lab2/locales.json");
        menu.chooseLanguage();

        while (true) {
            int task = menu.chooseTask();
            if (task == 0) {
                cout << menu.tr("bye") << "\n";
                return 0;
            }

            try {
                switch (task) {
                    case 1: runCheck(menu);   break;
                    case 2: runExtract(menu); break;
                    case 3: runForm(menu);    break;
                }
            } catch (const exception& e) {
                cerr << menu.tr("error_generic") << ": " << e.what() << "\n\n";
            }
        }
    } catch (const exception& e) {
        cerr << "Fatal: " << e.what() << "\n";
        return 1;
    }
}