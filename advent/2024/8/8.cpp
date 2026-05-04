#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

#include <iostream>
#include <vector>
#include <set>
#include <string>

// Function to check if a point is within the bounds of the map
bool is_within_bounds(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

// Function to find antinodes for a given pair of antennas
void find_antinode(int x1, int y1, int x2, int y2, std::set<std::pair<int, int>>& antinodes, int rows, int cols) {
    int dx = (x2 - x1) / 2;
    int dy = (y2 - y1) / 2;
    if (is_within_bounds(x1 - dx, y1 - dy, rows, cols)) {
        antinodes.insert({x1 - dx, y1 - dy});
    }
    if (is_within_bounds(x2 + dx, y2 + dy, rows, cols)) {
        antinodes.insert({x2 + dx, y2 + dy});
    }
}

// Main function
int main() {
    std::vector<std::string> map = {
        "............",
        "........0...",
        ".....0......",
        ".......0....",
        "....0.......",
        "......A.....",
        "............",
        "............",
        "........A...",
        ".........A..",
        "............",
        "............"
    };

    int rows = map.size();
    int cols = map[0].size();
    std::set<std::pair<int, int>> antinodes;

    // Find all antennas and check for pairs
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (map[i][j] != '.') {
                for (int x = 0; x < rows; ++x) {
                    for (int y = 0; y < cols; ++y) {
                        if (map[x][y] == map[i][j] && (x != i || y != j)) {
                            int dist_x = std::abs(x - i);
                            int dist_y = std::abs(y - j);
                            if (dist_x == 2 * dist_y || dist_y == 2 * dist_x) {
                                find_antinode(i, j, x, y, antinodes, rows, cols);
                            }
                        }
                    }
                }
            }
        }
    }

    std::cout << "Number of unique antinodes: " << antinodes.size() << std::endl;
    return 0;
}


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

