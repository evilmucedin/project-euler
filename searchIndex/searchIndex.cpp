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

int main() {
    cerr << filesystem::current_path() << endl;

    for (int file = 1; file < 10; ++file) {
        const string filename = string("searchIndex/") + to_string(file) + ".txt";
        cerr << filename << endl;

        std::ifstream istr(filename);
        cerr << "!" << istr.is_open() << endl;
        string s;
        while (readFromFile(istr, s)) {
            cerr << "  " << s << endl;
        }
    }
    return 0;
}
