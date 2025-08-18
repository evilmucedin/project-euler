#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>

using namespace std;

// Keypad layout
const vector<string> numericKeypad = {
    "789",
    "456",
    "123",
    " 0A"
};

const vector<string> directionalKeypad = {
    " ^A",
    "<v>"
};

// Directions for movement
const vector<pair<int, int>> directions = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}  // Down, Right, Up, Left
};

// Function to get the target button position on the numeric keypad
pair<int, int> findButtonPosition(const vector<string>& keypad, char button) {
    for (int row = 0; row < keypad.size(); ++row) {
        for (int col = 0; col < keypad[row].size(); ++col) {
            if (keypad[row][col] == button) {
                return {row, col};
            }
        }
    }
    return {-1, -1};  // Button not found
}

// Function to find the shortest path to press a button on the numeric keypad
int findShortestPathNumeric(const string& code) {
    auto [targetRow, targetCol] = findButtonPosition(numericKeypad, 'A');
    queue<tuple<int, int, string>> q;
    q.push({3, 2, ""});
    unordered_map<string, int> visited;
    visited["3,2"] = 0;

    while (!q.empty()) {
        auto [row, col, path] = q.front();
        q.pop();

        if (row == targetRow && col == targetCol) {
            if (path.back() != 'A') {
                path.push_back('A');
            }
            return path.length();
        }

        for (auto [dr, dc] : directions) {
            int newRow = row + dr;
            int newCol = col + dc;

            if (newRow >= 0 && newRow < numericKeypad.size() &&
                newCol >= 0 && newCol < numericKeypad[newRow].size() &&
                numericKeypad[newRow][newCol] != ' ') {
                string newPos = to_string(newRow) + "," + to_string(newCol);

                if (visited.find(newPos) == visited.end() || visited[newPos] > path.length() + 1) {
                    visited[newPos] = path.length() + 1;
                    q.push({newRow, newCol, path + "A"});
                }
            }
        }
    }

    return -1;  // Path not found
}

// Function to calculate the complexity of the code
long long int calculateComplexity(const string& code) {
    int numericPart = stoi(code.substr(0, code.length() - 1));
    int shortestPathLength = findShortestPathNumeric(code);
    return numericPart * shortestPathLength;
}

int main() {
    vector<string> codes = readInputLines();
    long long int totalComplexity = 0;

    for (const auto& code : codes) {
        auto cpx = calculateComplexity(code);
        totalComplexity += cpx;
        cerr << code << " " << cpx << endl;
    }

    cout << "Sum of the complexities of the five codes: " << totalComplexity << endl;

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
