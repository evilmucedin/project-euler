#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <sstream>

using namespace std;

const int GRID_SIZE = 71; // 71x71 grid

// Directions for moving up, down, left, right
const vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

// Function to simulate the falling bytes
void simulateFallingBytes(vector<vector<bool>>& grid, const vector<pair<int, int>>& bytes) {
    for (const auto& byte : bytes) {
        grid[byte.second][byte.first] = true;
    }
}

// Function to find the shortest path using BFS
int shortestPath(vector<vector<bool>>& grid) {
    queue<pair<int, int>> q;
    q.push({0, 0});

    vector<vector<bool>> visited(GRID_SIZE, vector<bool>(GRID_SIZE, false));
    visited[0][0] = true;

    int steps = 0;

    while (!q.empty()) {
        int size = q.size();
        for (int i = 0; i < size; ++i) {
            auto [x, y] = q.front();
            q.pop();

            if (x == GRID_SIZE - 1 && y == GRID_SIZE - 1) {
                return steps;
            }

            for (const auto& [dx, dy] : directions) {
                int nx = x + dx;
                int ny = y + dy;
                if (nx >= 0 && ny >= 0 && nx < GRID_SIZE && ny < GRID_SIZE && !grid[ny][nx] && !visited[ny][nx]) {
                    visited[ny][nx] = true;
                    q.push({nx, ny});
                }
            }
        }
        ++steps;
    }

    return -1; // If no path is found
}

int main() {
    // Input the list of bytes
    vector<pair<int, int>> bytes = {
        {5, 4}, {4, 2}, {4, 5}, {3, 0}, {2, 1}, {6, 3}, {2, 4}, {1, 5},
        {0, 6}, {3, 3}, {2, 6}, {5, 1}, {1, 2}, {5, 5}, {2, 5}, {6, 5},
        {1, 4}, {0, 4}, {6, 4}, {1, 1}, {6, 1}, {1, 0}, {0, 5}, {1, 6}, {

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
