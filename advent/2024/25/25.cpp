#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>

// Function to convert schematics to heights
std::vector<int> convertToHeights(const std::vector<std::string>& schematics, bool isLock) {
    std::vector<int> heights;
    for (size_t col = 0; col < schematics[0].size(); ++col) {
        int height = 0;
        if (isLock) {
            // Calculate lock pin heights from top down
            while (height < schematics.size() && schematics[height][col] == '#') {
                ++height;
            }
        } else {
            // Calculate key pin heights from bottom up
            int row = schematics.size() - 1;
            while (row >= 0 && schematics[row][col] == '#') {
                ++height;
                --row;
            }
        }
        heights.push_back(height);
    }
    return heights;
}

// Function to check if a key fits a lock
bool fits(const std::vector<int>& lockHeights, const std::vector<int>& keyHeights, int maxHeight) {
    for (size_t i = 0; i < lockHeights.size(); ++i) {
        if (lockHeights[i] + keyHeights[i] > maxHeight) {
            return false;
        }
    }
    return true;
}

int main() {
    std::vector<std::vector<std::string>> locks = {
        {
            "#####",
            ".####",
            ".####",
            ".####",
            ".#.#.",
            ".#...",
            "....."
        },
        {
            "#####",
            "##.##",
            ".#.##",
            "...##",
            "...#.",
            "...#.",
            "....."
        }
    };

    std::vector<std::vector<std::string>> keys = {
        {
            ".....",
            "#....",
            "#....",
            "#...#",
            "#.#.#",
            "#.###",
            "#####"
        },
        {
            ".....",
            ".....",
            "#.#..",
            "###..",
            "###.#",
            "###.#",
            "#####"
        },
        {
            ".....",
            ".....",
            ".....",
            "#....",
            "#.#..",
            "#.#.#",
            "#####"
        }
    };

    int maxHeight = locks[0].size();

    // Convert locks and keys to heights
    std::vector<std::vector<int>> lockHeights;
    for (const auto& lock : locks) {
        lockHeights.push_back(convertToHeights(lock, true));
    }

    std::vector<std::vector<int>> keyHeights;
    for (const auto& key : keys) {
        keyHeights.push_back(convertToHeights(key, false));
    }

    int validPairs = 0;

    // Check each key with each lock
    for (const auto& lock : lockHeights) {
        for (const auto& key : keyHeights) {
            if (fits(lock, key, maxHeight)) {
                ++validPairs;
            }
        }
    }

    std::cout << "Number of valid pairs: " << validPairs << std::endl;

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
