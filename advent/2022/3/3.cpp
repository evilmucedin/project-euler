#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

int pr(char ch) {
    if (ch >= 'a' && ch <= 'z') {
        return (ch - 'a') + 1;
    }
    if (ch >= 'A' && ch <= 'Z') {
        return (ch - 'A') + 27;
    }
    THROW("Bad ch");
}

string intersect(string s1, string s2) {
    string result;
    sort(s1.begin(), s1.end());
    s1.erase(unique(s1.begin(), s1.end()), s1.end());
    sort(s2.begin(), s2.end());
    int i = 0;
    int j = 0;
    while (i < s1.size() && j < s2.size()) {
        if (s1[i] < s2[j]) {
            ++i;
        } else if (s1[i] > s2[j]) {
            ++j;
        } else {
            result += s1[i];
            // cerr << s1[i] << endl;
            ++i;
            ++j;
        }
    }
    return result;
}

void first() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s: input) {
        if (s.empty()) {
            continue;
        }
        auto s1 = s.substr(0, s.size() / 2);
        auto s2 = s.substr(s.size() / 2, s.size() / 2);
        // cerr << s1 << endl;
        sort(s1.begin(), s1.end());
        s1.erase(unique(s1.begin(), s1.end()), s1.end());
        sort(s2.begin(), s2.end());
        for (char ch: intersect(s1, s2)) {
            result += pr(ch);
        }
    }
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    for (int i = 0; i < input.size(); i += 3) {
        auto s1 = input[i];
        auto s2 = input[i + 1];
        auto s3 = input[i + 2];
        auto s4 = intersect(s1, s2);
        auto s5 = intersect(s4, s3);
        for (char ch: s5) {
            result += pr(ch);
        }
    }
    cout << result << endl;
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

