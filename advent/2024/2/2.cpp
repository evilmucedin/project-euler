#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

bool small(int n) {
    return n == 1 || n == 2 || n == 3 || n == -1 || n == -2 || n == -3;
}

bool decOrInc(const vector<int>& nums) {
    int i = 0;
    for (i = 1; i < nums.size(); ++i) {
        if (nums[i - 1] < nums[i] || !small(nums[i - 1] - nums[i])) {
            break;
        }
    }
    if (i == nums.size()) {
        return true;
    }
    for (i = 1; i < nums.size(); ++i) {
        if (nums[i - 1] > nums[i] || !small(nums[i - 1] - nums[i])) {
            break;
        }
    }
    if (i == nums.size()) {
        return true;
    }
    return false;
}

bool decOrIncAlmoat(const vector<int>& nums) {
    if (decOrInc(nums)) {
        return true;
    }
    for (int i = 0; i < nums.size(); ++i) {
        auto cp = nums;
        cp.erase(cp.begin() + i);
        if (decOrInc(cp)) {
            return true;
        }
    }
    return false;
}

void first() {
    const auto input = readInputLines();
    int result = 0;

    for (int i = 0; i < input.size(); ++i) {
        const auto nums = parseIntegers(input[i]);
        result += decOrInc(nums);
    }

    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;

    for (int i = 0; i < input.size(); ++i) {
        const auto nums = parseIntegers(input[i]);
        result += decOrIncAlmoat(nums);
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

