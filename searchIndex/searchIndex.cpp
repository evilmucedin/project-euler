#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>

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

class Index {
private:
    using TIndex = map<string, vector<int>>;

public:
    void add(const string& key, int value) {
        auto toKey = index_.find(key);
        if (toKey == index_.end()) {
            index_.insert(std::make_pair(key, std::vector<int>{}));
        }
        toKey = index_.find(key);
        if (toKey->second.empty() || toKey->second.back() != value)
            toKey->second.push_back(value);
    }

private:
     TIndex index_;
};

int main() {
    cerr << filesystem::current_path() << endl;

    Index index;

    for (int file = 1; file < 10; ++file) {
        const string filename = string("searchIndex/") + to_string(file) + ".txt";
        cerr << filename << endl;

        std::ifstream istr(filename);
        cerr << "!" << istr.is_open() << endl;
        string s;
        while (readFromFile(istr, s)) {
            toLower(s);
            if (s.size() > 0) {
                index.add(s, file);
                cerr << "  " << s << endl;
            }
        }
    }
    return 0;
}
