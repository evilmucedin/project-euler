#!/bin/bash

# LeetCode Problem 984: String Without AAA or BBB
# This script generates the C++ solution file with test cases.

SOLUTION_DIR="leetcode_solutions/984-string-without-aaa-or-bbb"
FILE_NAME="${SOLUTION_DIR}/solution.cpp"
README_FILE="${SOLUTION_DIR}/README.md"

# Create directory if it doesn't exist
mkdir -p "$SOLUTION_DIR"

# Write the C++ solution to file
cat > "$FILE_NAME" << 'EOF'
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class Solution {
public:
    string strWithout3a3b(int a, int b) {
        string result = "";
        
        while (a > 0 || b > 0) {
            if (a == b) {
                // Equal counts: add "ab" to keep balance
                result += "ab";
                a--; 
                b--;
            } else if (a > b) {
                // More 'a's remaining
                if (a >= b + 2) {
                    // Safe to add 2 'a's (guaranteed no 'aaa')
                    result += "aa";
                    a -= 2;
                } else {
                    // Only 1 'a' needed to maintain balance
                    result += "a";
                    a--;
                }
            } else {
                // More 'b's remaining
                if (b >= a + 2) {
                    // Safe to add 2 'b's (guaranteed no 'bbb')
                    result += "bb";
                    b -= 2;
                } else {
                    // Only 1 'b' needed
                    result += "b";
                    b--;
                }
            }
        }
        
        return result;
    }
};

// Test cases
void runTest(const string& name, int a, int b) {
    Solution solution;
    string result = solution.strWithout3a3b(a, b);
    
    // Validate output
    bool valid = true;
    if (result.find("aaa") != string::npos || 
        result.find("bbb") != string::npos) {
        valid = false;
    }
    
    int countA = 0, countB = 0;
    for (char c : result) {
        if (c == 'a') countA++;
        else if (c == 'b') countB++;
    }
    
    if (countA != a || countB != b) {
        valid = false;
    }
    
    cout << "\033[1mTest: " << name << "\033[0m" << endl;
    cout << "  Input:  a=" << a << ", b=" << b << endl;
    cout << "  Output: \"" << result << "\"" << endl;
    cout << "  Length: " << result.length() << " (expected: " << a + b << ")" << endl;
    cout << "  CountA: " << countA << ", CountB: " << countB << endl;
    cout << "  Valid:  \033[" << (valid ? "32m✓\033[0m" : "31m✗\033[0m") << endl;
}

int main() {
    runTest("Example 1", 1, 2);      // Expected: "abb" or similar
    runTest("Example 2", 4, 1);      // Expected: "aabaa"
    runTest("Balanced", 3, 3);       // Expected: "ababab"
    runTest("Edge Case 1", 0, 2);    // Expected: "bb"
    runTest("Edge Case 2", 2, 0);    // Expected: "aa"
    runTest("Large", 10, 5);         // Custom large case
    runTest("Equal Large", 7, 7);    // Equal counts
    
    return 0;
}
EOF

