#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLinesAll();
    long long mx = 0;
    long long cur = 0;
    for (auto& s: input) {
        // s = trim(s);
        if (!s.empty()) {
            cur += atoi(s.c_str());
        } else {
            mx = max(mx, cur);
            cur = 0;
        }
    }
    cout << mx << endl;
}

void second() {
    const auto input = readInputLinesAll();
    vector<long long> all;
    long long cur = 0;
    for (auto& s: input) {
        // s = trim(s);
        if (!s.empty()) {
            cur += atoi(s.c_str());
        } else {
            all.emplace_back(cur);
            cur = 0;
        }
    }
    all.emplace_back(cur);
    sort(all.begin(), all.end());
    reverse(all);
    cout << all[0] + all[1] + all[2] << endl;
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

