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
    long long result2 = 0;
    unordered_map<int, int> counts;
    for (const auto& s: input) {
        const auto parts = split(s, '|');
        const auto nums1 = parseIntegers(parts[0]);
        int cNum = nums1[0];
        ++counts[cNum];
        const auto nums2 = parseIntegers(parts[1]);
        unordered_set<int> sNums1(nums1.begin() + 1, nums1.end());
        int over = 0;
        for (auto n: nums2) {
            over += sNums1.count(n);
        }
        if (over) {
            for (int i = 0; i < over; ++i) {
                counts[cNum + i + 1] += counts[cNum];
            }
            result += 1 << (over - 1);
            result2 += (1 << (over - 1)) * counts[cNum];
        }
    }
    int result3 = 0;
    for (const auto& kv: counts) {
        result3 += kv.second;
    }
    cout << result << endl;
    cout << result2 << endl;
    cout << result3 << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
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

