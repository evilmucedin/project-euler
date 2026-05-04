#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

// Define the directions and colors as constants
const vector<pair<int, int>> DIRS = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
const unordered_map<char, string> COLORS = {
    {'R', "#70c710"},
    {'D', "#0dc571"},
    {'L', "#5713f0"},
    {'U', "#d2c081"},
    {'r', "#59c680"},
    {'d', "#411b91"},
    {'l', "#8ceee2"},
    {'u', "#caa173"}
};

// Define the grid size and the empty value
const int N = 100;
const int EMPTY = -1;

// A function to fill the interior of the lagoon with lava
void fill(int x, int y, int z, vector<vector<vector<int>>>& grid, int& lava) {
    // Check if the coordinates are valid and the cell is empty
    if (x < 0 || x >= N || y < 0 || y >= N || z < 0 || z >= N || grid[x][y][z] != EMPTY) {
        return;
    }
    // Fill the cell with lava and increment the lava count
    grid[x][y][z] = 1;
    lava++;
    // Recursively fill the adjacent cells
    for (auto dir : DIRS) {
        fill(x + dir.first, y + dir.second, z, grid, lava);
    }
}

void first() {
    const auto input = readInputLines();

    // Initialize the grid with empty values
    vector<vector<vector<int>>> grid(N, vector<vector<int>>(N, vector<int>(N, EMPTY)));
    // Read the input and parse the commands
    int x = N/2, y = N/2, z = 0;
    for (const auto& line: input) {
        char dir = line[0];
        int len = stoi(line.substr(2));
        string color = line.substr(9);
        // Start from the origin
        // Move according to the direction and length, and paint the edge
        for (int i = 0; i < len; i++) {
            if (dir == 'R' || dir == 'r') x++;
            if (dir == 'L' || dir == 'l') x--;
            if (dir == 'U' || dir == 'u') y++;
            if (dir == 'D' || dir == 'd') y--;
            grid[x][y][z] = 0;
            cout << "Painted (" << x << "," << y << "," << z << ") with " << COLORS.at(dir) << "\n";
        }
    }
    // Initialize the lava count
    int lava = 0;
    // Fill the interior of the lagoon with lava
    fill(x, y, 0, grid, lava);
    // Output the result
    cout << "The lagoon can hold " << lava << " cubic meters of lava.\n";
}

struct Point {
    int x, y;
};

void first_() {
    const auto input = readInputLines();

    std::vector<std::string> commands;
    std::unordered_map<std::string, Point> colors;

    for (int i = 0; i < input.size(); ++i) {
        std::istringstream lineStream(input[i]);
        std::string direction, color;
        int steps;

        lineStream >> direction >> steps >> color;

        commands.push_back(direction);
        colors[color] = {0, 0}; // Initialize all colors with (0, 0)
    }

    // Process commands and update color coordinates
    int max_x = 0, min_x = 0, max_y = 0, min_y = 0;
    int current_x = 0, current_y = 0;

    for (const auto& cmd : input) {
        char direction = cmd[0];
        int steps = std::stoi(cmd.substr(2));

        for (int i = 0; i < steps; ++i) {
            switch (direction) {
                case 'U':
                    current_y++;
                    max_y = std::max(max_y, current_y);
                    break;
                case 'D':
                    current_y--;
                    min_y = std::min(min_y, current_y);
                    break;
                case 'L':
                    current_x--;
                    min_x = std::min(min_x, current_x);
                    break;
                case 'R':
                    current_x++;
                    max_x = std::max(max_x, current_x);
                    break;
                default:
                    break;
            }
        }
    }

    // Calculate area and depth of the lagoon
    int width = max_x - min_x + 1;
    int height = max_y - min_y + 1;
    int depth = std::min(abs(min_x), abs(max_x)) + std::min(abs(min_y), abs(max_y));

    int lagoon_volume = width * height * depth;
    cout << lagoon_volume << endl;
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

