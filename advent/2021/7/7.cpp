#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    const auto pos = stringVectorCast<int>(split(input[0], ','));
    const auto mn = minV(pos);
    const auto mx = maxV(pos);

    i64 optF = 100000000000;

    FOR (i, mn, mx + 1) {
        i64 f = 0;
        for (const auto& p: pos) {
            f += std::abs(p - i);
        }
        if (f < optF) {
            optF = f;
        }
    }

    cout << optF << endl;
}

void second() {
    const auto input = readInputLines();
    const auto pos = stringVectorCast<int>(split(input[0], ','));
    const auto mn = minV(pos);
    const auto mx = maxV(pos);

    i64 optF = 100000000000;

    FOR (i, mn, mx + 1) {
        i64 f = 0;
        for (const auto& p: pos) {
            int s = std::abs(p - i);
            f += s * (s + 1);
        }
        if (f < optF) {
            optF = f;
        }
    }

    cout << optF / 2 << endl;
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

