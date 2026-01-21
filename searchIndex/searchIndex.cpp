#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;

bool readFromFile(std::ifstream& file, string& s) {
    s.clear();
    while (s.size() == 0) {
        if (!(file >> s))
            return false;
    }
    return true;
}

bool isChar(char ch) {
    return ch >= 'a' && ch <= 'z';
}

void toLower(string& s) {
    const auto n = s.length();
    for (int i = 0; i < n; ++i)
        s[i] = tolower(s[i]);
    while (s.size() && !isChar(s[s.size() - 1]))
        s.pop_back();
}

int main() {
    cerr << filesystem::current_path() << endl;

    for (int file = 1; file < 10; ++file) {
        const string filename = string("searchIndex/") + to_string(file) + ".txt";
        cerr << filename << endl;

        std::ifstream istr(filename);
        cerr << "!" << istr.is_open() << endl;
        string s;
        while (readFromFile(istr, s)) {
            toLower(s);
            if (s.size() > 0) {
                cerr << "  " << s << endl;
            }
        }
    }
    return 0;
}
