#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "lib/header.h"
#include "lib/string.h"

#include "advent/lib/aoc.h"

int main() {
    // Example input lists
    std::vector<long long int> left = {3, 4, 2, 1, 3, 3};
    std::vector<long long int> right = {4, 3, 5, 3, 9, 3};

    const auto lines = readInputLines();
    left.clear();
    right.clear();
    for (const auto& s: lines) {
        const auto ints = parseIntegers(s);
        if (ints.size() == 1) {
            left.emplace_back(ints[0]);
        } else if (ints.size() == 2) {
            left.emplace_back(ints[0]);
            right.emplace_back(ints[1]);
        } else {
            throw 1;
        }
    }
    std::cerr << left << endl;
    std::cerr << right << endl;

    // Sort the lists
    std::sort(left.begin(), left.end());
    std::sort(right.begin(), right.end());

    // Calculate the total distance
    long long int totalDistance = 0;
    for (size_t i = 0; i < left.size(); ++i) {
        totalDistance += std::abs(left[i] - right[i]);
    }

    // Output the result
    std::cout << "Total distance: " << totalDistance << std::endl;

    return 0;
}

