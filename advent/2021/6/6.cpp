#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto strings = readInputLines();
    const auto parts = split(strings[0], ',');

    vector<i64> counts(9, 0);
    for (const auto& s: parts) {
        ++counts[stoi(s)];
    }

    for (size_t i = 0; i < 80; ++i) {
        vector<i64> newCounts(9, 0);
        for (size_t i = 1; i < 9; ++i) {
            newCounts[i - 1] += counts[i];
        }
        newCounts[8] += counts[0];
        newCounts[6] += counts[0];
        swap(counts, newCounts);
    }
    cout << sum(counts) << endl;
}

void second() {
    const auto strings = readInputLines();
    const auto parts = split(strings[0], ',');

    vector<i64> counts(9, 0);
    for (const auto& s: parts) {
        ++counts[stoi(s)];
    }

    for (size_t i = 0; i < 256; ++i) {
        vector<i64> newCounts(9, 0);
        for (size_t i = 1; i < 9; ++i) {
            newCounts[i - 1] += counts[i];
        }
        newCounts[8] += counts[0];
        newCounts[6] += counts[0];
        swap(counts, newCounts);
    }
    cout << sum(counts) << endl;
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

