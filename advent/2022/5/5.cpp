#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLinesAll();
    const int n = 9;
    vector<vector<char>> st(n);

    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 9; ++j) {
            if ((input[i].size() > 4*j + 1) && (input[i][4*j + 1] != ' ')) {
                st[j].push_back(input[i][4*j + 1]);
            }
        }
    }

    for (const string& s: input) {
        const auto ints = parseIntegers(s);
        if (ints.size() == 3) {
            for (int i = 0; i < ints[0]; ++i) {
                st[ints[2] - 1].push_back(st[ints[1] - 1].back());
                st[ints[1] - 1].pop_back();
            }
        }
    }

    string s;
    for (int i = 0; i < 9; ++i) {
        s += st[i].back();
    }

    cout << s << endl;
}

void second() {
    const auto input = readInputLinesAll();
    const int n = 9;
    vector<vector<char>> st(n);

    for (int i = 7; i >= 0; --i) {
        for (int j = 0; j < 9; ++j) {
            if ((input[i].size() > 4*j + 1) && (input[i][4*j + 1] != ' ')) {
                st[j].push_back(input[i][4*j + 1]);
            }
        }
    }

    for (const string& s: input) {
        const auto ints = parseIntegers(s);
        if (ints.size() == 3) {
            auto& from = st[ints[1] - 1];
            auto& to = st[ints[2] - 1];
            to.insert(to.end(), from.begin() + (from.size() - ints[0]), from.end());
            from.erase(from.begin() + (from.size() - ints[0]), from.end());
        }
    }

    string s;
    for (int i = 0; i < 9; ++i) {
        s += st[i].back();
    }

    cout << s << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

