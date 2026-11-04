#include "menu.h"

using nlohmann::json;
using namespace std;

Menu::Menu(const string& jsonPath) {
    ifstream f(jsonPath);
    if (!f.is_open()) {
        throw runtime_error("error: json wasnt open " + jsonPath);
    }
    f >> data;
}

string Menu::tr(const string& key) const {
    auto it = current.find(key);
    if (it != current.end() && it->is_string()) {
        return it->get<string>();
    }
    return key;
}

int Menu::readInt(const string& prompt, int minV, int maxV) {
    while (true) {
        cout << prompt;
        int x;
        if (cin >> x && x >= minV && x <= maxV) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return x;
        }
        cerr << tr("err_input") << "\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int Menu::chooseLanguage() {
    cout << "Выберите язык / Choose language / Sprache wählen:\n";
    cout << "1. Русский\n";
    cout << "2. English\n";
    cout << "3. Deutsch\n";

    int choice;
    while (true) {
        cout << "> ";
        if (cin >> choice && choice >= 1 && choice <= 3) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            break;
        }
        cerr << "Ошибка / Error / Fehler\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    if (choice == 1) lang = "ru";
    else if (choice == 2) lang = "en";
    else lang = "de";

    current = data[lang];
    return choice;
}

int Menu::chooseTask() {
    cout << "\n" << tr("menu_title") << "\n";
    cout << "1. " << tr("task1") << "\n";
    cout << "2. " << tr("task2") << "\n";
    cout << "3. " << tr("task3") << "\n";
    cout << "0. " << tr("exit")  << "\n";
    return readInt(tr("prompt"), 0, 3);
}