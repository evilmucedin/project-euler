#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

#include <iostream>
#include <vector>
#include <string>
using namespace std;

unordered_map<string, long long> cache;

// Function to count the number of valid arrangements for a row
long long countArrangementsi4(const string& row, const vector<int>& damagedGroups) {
    if (damagedGroups.size() == 0) {
        int count = 0;
        for (char ch : row) {
            if (ch == '?') {
                ++count;
            }
        }
        return 1 << count;
    }

    const string key = row + "|" + to_string(damagedGroups);
    auto toKey = cache.find(key);
    if (toKey != cache.end()) {
        return toKey->second;
    }

    :q

}


// Function to count the number of valid arrangements for a row
long long countArrangementsi4(const string& row, const vector<int>& damagedGroups) {
    int unknownCount = 0;
    for (char c : row) {
        if (c == '?') {
            unknownCount++;
        }
    }

    // Calculate the maximum number of configurations for unknown '?' symbols
    long long maxConfigurations = 1;
    for (int groupSize : damagedGroups) {
        for (int i = 1; i <= groupSize; ++i) {
            maxConfigurations *= (unknownCount + i);
            maxConfigurations /= i;
        }
        unknownCount -= groupSize;
    }

    LOG(INFO) << OUT(row) << OUT(damagedGroups) << OUT(maxConfigurations);
    return maxConfigurations;
}

// Function to count the number of valid arrangements for a row
long long countArrangements3(const string& row, const vector<int>& damagedGroups) {
    vector<long long> dp(row.size() + 1);
    dp[0] = 1;

    for (int i = 0; i < row.size(); ++i) {
        if (row[i] == '.') {
            for (int groupSize : damagedGroups) {
                if (i + groupSize <= row.size()) {
                    dp[i + groupSize] += dp[i];
                }
            }
        }
    }

    LOG(INFO) << OUT(row) << OUT(damagedGroups) << OUT(dp.back());

    return dp[row.size()];
}

// Function to count the number of valid arrangements for a row
int countArrangements2(const string& row, const vector<int>& damagedGroups) {
    int totalArrangements = 1;

    for (int groupSize : damagedGroups) {
        int count = 0;
        for (char c : row) {
            if (c == '?') {
                count++;
            } else {
                if (count >= groupSize) {
                    totalArrangements *= (count - groupSize + 1);
                }
                count = 0;
            }
        }
        if (count >= groupSize) {
            totalArrangements *= (count - groupSize + 1);
        }
    }

    LOG(INFO) << OUT(row) << OUT(damagedGroups) << OUT(totalArrangements);

    return totalArrangements;
}

// Function to calculate the total count of valid arrangements for all rows
int totalArrangementsCount(const vector<string>& rows, const vector<vector<int>>& damagedGroupsList) {
    int totalCount = 0;
    for (size_t i = 0; i < rows.size(); ++i) {
        totalCount += countArrangements(rows[i], damagedGroupsList[i]);
    }
    return totalCount;
}


void first() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s: input) {
        const auto parts = split(s, ' ');
        result += countArrangements(parts[0], parseIntegers(parts[1]));
    }
    cout << result << endl;
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

