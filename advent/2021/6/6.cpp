#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

i64 solve(int its) {
    const auto strings = readInputLines();
    const auto parts = split(strings[0], ',');

    vector<i64> counts(9, 0);
    for (const auto& s : parts) {
        ++counts[stoi(s)];
    }

    for (size_t i = 0; i < its; ++i) {
        vector<i64> newCounts(9, 0);
        for (size_t i = 1; i < 9; ++i) {
            newCounts[i - 1] += counts[i];
        }
        newCounts[8] += counts[0];
        newCounts[6] += counts[0];
        swap(counts, newCounts);
    }

    return sum(counts);
}

void first() { cout << solve(80) << endl; }

void second() { cout << solve(256) << endl; }

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
