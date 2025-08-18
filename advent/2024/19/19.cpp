#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <string>
#include <queue>

using namespace std;

// Function to check if a design can be made using available towel patterns
bool canCreateDesign(const string& design, const unordered_set<string>& towelPatterns) {
    int n = design.size();
    vector<bool> dp(n + 1, false);
    dp[0] = true;

    for (int i = 1; i <= n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (dp[j] && towelPatterns.find(design.substr(j, i - j)) != towelPatterns.end()) {
                dp[i] = true;
                break;
            }
        }
    }

    return dp[n];
}

int main() {
    const auto lines = readInputLines();
    // List of available towel patterns
    vector<string> towelPatternsList = {"r", "wr", "b", "g", "bwu", "rb", "gb", "br"};
    towelPatternsList = split(lines[0], ',');
    unordered_set<string> towelPatterns(towelPatternsList.begin(), towelPatternsList.end());

    // List of designs the onsen would like to display
    vector<string> designs = {
        "brwrr", "bggr", "gbbr", "rrbgbr", "ubwu", "bwurrg", "brgr", "bbrgwb"
    };
    designs.clear();
    for (int i = 2; i < lines.size(); ++i) {
        designs.emplace_back(lines[i]);
    }

    int possibleDesigns = 0;

    for (const string& design : designs) {
        if (canCreateDesign(design, towelPatterns)) {
            possibleDesigns++;
        }
    }

    cout << "Number of possible designs: " << possibleDesigns << endl;

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
