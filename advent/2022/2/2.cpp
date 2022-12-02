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
    for (const auto& s : input) {
        auto parts = split(s, ' ');
        if (parts[1] == "X") {
            result += 1;
            if (parts[0] == "C") {
                result += 6;
            } else if (parts[0] == "A") {
                result += 3;
            }
        } else if (parts[1] == "Y") {
            result += 2;
            if (parts[0] == "A") {
                result += 6;
            } else if (parts[0] == "B") {
                result += 3;
            }
        } else if (parts[1] == "Z") {
            result += 3;
            if (parts[0] == "B") {
                result += 6;
            } else if (parts[0] == "C") {
                result += 3;
            }
        } else {
            THROW("Bad input");
        }
    }
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s : input) {
        auto parts = split(s, ' ');
        if (parts[1] == "X") {
            result += 0;
            if (parts[0] == "A") {
                result += 3;
            } else if (parts[0] == "B") {
                result += 1;
            } else if (parts[0] == "C") {
                result += 2;
            }
        } else if (parts[1] == "Y") {
            result += 3;
            if (parts[0] == "A") {
                result += 1;
            } else if (parts[0] == "B") {
                result += 2;
            } else if (parts[0] == "C") {
                result += 3;
            }
        } else if (parts[1] == "Z") {
            result += 6;
            if (parts[0] == "A") {
                result += 2;
            } else if (parts[0] == "B") {
                result += 3;
            } else if (parts[0] == "C") {
                result += 1;
            }
        } else {
            THROW("Bad input");
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

