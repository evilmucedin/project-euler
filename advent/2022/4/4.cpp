#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s: input) {
        const auto s2 = replaceAll(s, "-", "*");
        const auto ints = parseIntegers(s2);
        // cerr << ints << endl;
        int a1 = ints[0];
        int a2 = ints[1];
        int b1 = ints[2];
        int b2 = ints[3];
        if (a1 >= b1 && a2 <= b2) {
            ++result;
        } else if (b1 >= a1 && b2 <= a2) {
            ++result;
        }
    }
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s: input) {
        const auto s2 = replaceAll(s, "-", "*");
        const auto ints = parseIntegers(s2);
        // cerr << ints << endl;
        int a1 = ints[0];
        int a2 = ints[1];
        int b1 = ints[2];
        int b2 = ints[3];
        if (a1 >= b1 && a1 <= b2) {
            ++result;
        } else if (b1 >= a1 && b1 <= a2) {
            ++result;
        } else if (a2 >= b1 && a2 <= b2) {
            ++result;
        } else if (b2 >= a1 && b2 <= a2) {
            ++result;
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

