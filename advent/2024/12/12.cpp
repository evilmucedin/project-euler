#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <queue>
#include <set>
#include <utility>

// Function to check if a given position is within the bounds of the map
bool is_within_bounds(int x, int y, int rows, int cols) {
    return x >= 0 && x < rows && y >= 0 && y < cols;
}

unordered_set<pair<int, int>> used;

// Function to perform BFS and calculate area and perimeter of a region
std::tuple<int, int, int> bfs(const std::vector<std::string>& map, int start_x, int start_y, char plant_type) {
    int rows = map.size();
    int cols = map[0].size();
    std::vector<std::vector<bool>> visited(rows, std::vector<bool>(cols, false));
    std::queue<std::pair<int, int>> q;
    q.push({start_x, start_y});
    visited[start_x][start_y] = true;

    int area = 0;
    int perimeter = 0;
    int sides = 0;
    // Directions for moving up, down, left, right
    std::vector<std::pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        area++;
        used.emplace(make_pair(x, y));

        for (const auto& [dx, dy] : directions) {
            int new_x = x + dx;
            int new_y = y + dy;
            if (is_within_bounds(new_x, new_y, rows, cols) && map[new_x][new_y] == plant_type) {
                if (!visited[new_x][new_y]) {
                    visited[new_x][new_y] = true;
                    q.push({new_x, new_y});
                }
            } else {
                perimeter++;
                ++sides;
            }
        }
    }

    return {area, perimeter, sides};
}

int main() {
    std::vector<std::string> garden_map = {"RRRRIICCFF", "RRRRIICCCF", "VVRRRCCFFF", "VVRCCCJFFF", "VVVVCJJCFE",
                                           "VVIVCCJJEE", "VVIIICJJEE", "MIIIIIJJEE", "MIIISIJEEE", "MMMISSJEEE"};
                                           garden_map = readInputLines();

    int rows = garden_map.size();
    int cols = garden_map[0].size();
    std::unordered_map<char, bool> visited;

    long long int total_price = 0;
    long long int total_price2 = 0;

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            char plant_type = garden_map[i][j];
            if (used.find(make_pair(i, j)) == used.end() && visited[plant_type] == false) {
                auto [area, perimeter, sides] = bfs(garden_map, i, j, plant_type);
                long long int price = area * perimeter;
                total_price += price;
                total_price2 += area * sides;
                // visited[plant_type] = true;
                std::cout << "Region of " << plant_type << " plants with area " << area << " and perimeter "
                          << perimeter << " has price " << price << ".\n";
            }
        }
    }

    std::cout << "Total price of fencing all regions: " << total_price << std::endl;
    std::cout << "Total price2 of fencing all regions: " << total_price2 << std::endl;

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
