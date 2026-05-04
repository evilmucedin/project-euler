#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

bool gg(const string& s) {
    const auto parts1 = split(s, ':');
    const auto parts2 = split(parts1[1], ';');
    for (const auto& p: parts2) {
        auto parts3 = split(p, ' ');
        for (int i = 0; i < parts3.size(); ++i) {
            if (parts3[i].back() == ',') {
                parts3[i] = parts3[i].substr(0, parts3[i].size() - 1);
            }
            if (parts3[i] == "red") {
                if (atoi(parts3[i - 1].c_str()) > 12) {
                    return false;
                }
            }
            else if (parts3[i] == "green") {
                if (atoi(parts3[i - 1].c_str()) > 13) {
                    return false;
                }
            }
            else if (parts3[i] == "blue") {
                if (atoi(parts3[i - 1].c_str()) > 14) {
                    return false;
                }
            } else if (!isInteger(parts3[i])) {
                LOG(ERROR) << parts3[i];
            }
        }
    }
    return true;
}

long long gg2(const string& s) {
    int mb = 0;
    int mr = 0;
    int mg = 0;


    const auto parts1 = split(s, ':');
    const auto parts2 = split(parts1[1], ';');
    for (const auto& p: parts2) {
        auto parts3 = split(p, ' ');
        for (int i = 0; i < parts3.size(); ++i) {
            if (parts3[i].back() == ',') {
                parts3[i] = parts3[i].substr(0, parts3[i].size() - 1);
            }
            if (parts3[i] == "red") {
                mr = max(mr, atoi(parts3[i - 1].c_str()));
            }
            else if (parts3[i] == "green") {
                mg = max(mg, atoi(parts3[i - 1].c_str()));
            }
            else if (parts3[i] == "blue") {
                mb = max(mb, atoi(parts3[i - 1].c_str()));
            } else if (!isInteger(parts3[i])) {
                LOG(ERROR) << parts3[i];
            }
        }
    }
    return mr*mb*mg;
}

void first() {
    const auto input = readInputLines();
    int result = 0;
    for (const string& s: input) {
        const auto ints = parseIntegers(s);
        if (gg(s)) {
            result += ints[0];
        }
    }
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    long long result = 0;
    for (const string& s: input) {
        result += gg2(s);
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

