#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

using T = __int128;
const int MAX_STEPS = 64;
unordered_map<long long, T> cache;

T dfs(vector<string>& grid, long long x, long long y, long long steps) {
    // Check if the current position is outside the grid or if it's not a garden plot
    if (x < 0 || x >= grid.size() || y < 0 || y >= grid[0].size() || grid[x][y] == '#')
        return 0;

    // If steps run out, return 1 if the current position is a garden plot, else return 0
    if (steps == 0)
        return (grid[x][y] == '.') ? 1 : 0;

    long long key = (long long)x + 100000*(long long)y + (long long)steps*10000000;
    auto toKey = cache.find(key);
    if (toKey != cache.end()) {
        return toKey->second;
    }

    T count = 0;
    // Mark the current position as visited
    char original = grid[x][y];
    grid[x][y] = '#';

    // Explore all possible directions
    count += dfs(grid, x + 1, y, steps - 1); // Move right
    count += dfs(grid, x - 1, y, steps - 1); // Move left
    count += dfs(grid, x, y + 1, steps - 1); // Move down
    count += dfs(grid, x, y - 1, steps - 1); // Move up

    // Restore the grid to its original state
    grid[x][y] = original;

    cache.emplace(key, count);
    return count;
}

std::string toString(__int128 num) {
    std::string str;
    do {
        int digit = num % 10;
        str = std::to_string(digit) + str;
        num = (num - digit) / 10;
    } while (num != 0);
    return str;
}

void first() {
    auto grid = readInputLines();

    int rows = grid.size();
    int cols = grid[0].size();
    int start_x, start_y;

    // Find the starting position (S)
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 'S') {
                start_x = i;
                start_y = j;
                break;
            }
        }
    }

    // Calculate the number of garden plots the Elf can reach in exactly 64 steps
    grid[start_x][start_y] = '.';
    auto result = dfs(grid, start_x, start_y, MAX_STEPS);
    cout << "The number of garden plots the Elf can reach in exactly 64 steps: " << toString(result) << endl;
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

