#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <string>

// Function to split a stone with an even number of digits into two stones
std::vector<int> split_stone(int stone) {
    std::string str = std::to_string(stone);
    int mid = str.size() / 2;
    int left = std::stoi(str.substr(0, mid));
    int right = std::stoi(str.substr(mid));
    return {left, right};
}

// Function to process the stones for one blink
std::vector<int> blink(std::vector<int>& stones) {
    std::vector<int> new_stones;
    for (int stone : stones) {
        if (stone == 0) {
            new_stones.push_back(1);
        } else if (std::to_string(stone).size() % 2 == 0) {
            std::vector<int> split = split_stone(stone);
            new_stones.insert(new_stones.end(), split.begin(), split.end());
        } else {
            new_stones.push_back(stone * 2024);
        }
    }
    return new_stones;
}

int main() {
    std::vector<int> stones = {125, 17};  // Initial arrangement of stones
    stones = parseIntegers(readInputLines().front());

    int blinks = 25;  // Number of blinks
    for (int i = 0; i < blinks; ++i) {
        stones = blink(stones);
    }

    std::cout << "Number of stones after " << blinks << " blinks: " << stones.size() << std::endl;
    return 0;
}

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
}

/*
int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

    */
