#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

int extrapolateNextValue(std::vector<long long> sequence) {
    std::vector<long long> differences;
    for (size_t i = 1; i < sequence.size(); ++i) {
        differences.push_back(sequence[i] - sequence[i - 1]);
    }

    bool allZeroes = true;
    for (int diff : sequence) {
        if (diff != 0) {
            allZeroes = false;
            break;
        }
    }

    if (allZeroes) {
        return 0;
    } else {
        return sequence.back() + extrapolateNextValue(differences);
    }
}

void first() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& l: input) {
        const auto ints = parseIntegers<long long>(l);
        if (ints.size()) {
            // cerr << ints << endl;
            // cerr << extrapolateNextValue(ints) << endl;
            result += extrapolateNextValue(ints);
        }
    }
    cout << result << endl;
}

int extrapolatePrevValue(std::vector<long long> sequence) {
    std::vector<long long> differences;
    for (size_t i = 1; i < sequence.size(); ++i) {
        differences.push_back(sequence[i] - sequence[i - 1]);
    }

    bool allZeroes = true;
    for (int diff : sequence) {
        if (diff != 0) {
            allZeroes = false;
            break;
        }
    }

    if (allZeroes) {
        return 0;
    } else {
        return sequence.front() - extrapolatePrevValue(differences);
    }
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& l: input) {
        const auto ints = parseIntegers<long long>(l);
        if (ints.size()) {
            // cerr << ints << endl;
            // cerr << extrapolatePrevValue(ints) << endl;
            result += extrapolatePrevValue(ints);
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

