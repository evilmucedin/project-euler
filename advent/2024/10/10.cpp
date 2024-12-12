#include <iostream>
#include <queue>
#include <set>
#include <vector>

#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/exception.h"
#include "lib/init.h"
#include "lib/string.h"

// Function to check if a given position is within the bounds of the map
bool is_within_bounds(int x, int y, int rows, int cols) { return x >= 0 && x < rows && y >= 0 && y < cols; }

// Function to find all trailheads (positions with height 0)
std::vector<std::pair<int, int>> find_trailheads(const std::vector<std::vector<int>>& map) {
    std::vector<std::pair<int, int>> trailheads;
    for (int i = 0; i < map.size(); ++i) {
        for (int j = 0; j < map[0].size(); ++j) {
            if (map[i][j] == 0) {
                trailheads.push_back({i, j});
            }
        }
    }
    return trailheads;
}

// Function to perform BFS to find all reachable 9-height positions from a given trailhead
int bfs(const std::vector<std::vector<int>>& map, int start_x, int start_y) {
    int rows = map.size();
    int cols = map[0].size();
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<std::pair<int, int>> q;
    q.push({start_x, start_y});
    visited[start_x][start_y] = true;
    std::set<std::pair<int, int>> reachable_nines;

    // Direction vectors for moving up, down, left, right
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();

        for (const auto& [dx, dy] : directions) {
            int new_x = x + dx;
            int new_y = y + dy;
            if (is_within_bounds(new_x, new_y, rows, cols) && !visited[new_x][new_y]) {
                if (map[new_x][new_y] == map[x][y] + 1) {
                    visited[new_x][new_y] = true;
                    q.push({new_x, new_y});
                    if (map[new_x][new_y] == 9) {
                        reachable_nines.insert({new_x, new_y});
                    }
                }
            }
        }
    }
    return reachable_nines.size();
}

// Main function
int main() {
    std::vector<std::vector<int>> topographic_map = {
        {8, 9, 0, 1, 0, 1, 2, 3}, {7, 8, 1, 2, 1, 8, 7, 4}, {8, 7, 4, 3, 0, 9, 6, 5}, {9, 6, 5, 4, 9, 8, 7, 4},
        {4, 5, 6, 7, 8, 9, 0, 3}, {3, 2, 0, 1, 9, 0, 1, 2}, {0, 1, 3, 2, 9, 8, 0, 1}, {1, 0, 4, 5, 6, 7, 3, 2}};

    topographic_map.clear();
    const auto input = readInputLines();
    for (const auto& s: input) {
        vector<int> line;
        for (auto ch: s) {
            line.emplace_back(ch - '0');
        }
        topographic_map.emplace_back(std::move(line));
    }

    std::vector<std::pair<int, int>> trailheads = find_trailheads(topographic_map);
    int total_score = 0;

    for (const auto& [x, y] : trailheads) {
        total_score += bfs(topographic_map, x, y);
    }

    std::cout << "Sum of the scores of all trailheads: " << total_score << std::endl;

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

