#include <set>

#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int index = 0;
    while (index + 4 < input[0].size()) {
        if (set(input[0].begin() + index, input[0].begin() + index + 4).size() == 4) {
            cout << index + 4 << endl;
            break;
        }
        ++index;
    }
}

void second() {
    const auto input = readInputLines();
    int index = 0;
    while (index + 14 < input[0].size()) {
        if (set(input[0].begin() + index, input[0].begin() + index + 14).size() == 14) {
            cout << index + 14 << endl;
            break;
        }
        ++index;
    }
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

