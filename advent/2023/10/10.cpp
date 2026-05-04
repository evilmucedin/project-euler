#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/exception.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

// Structure to represent a point in the grid
struct Point {
    int x, y;
};

// Function to check if the given coordinates are within bounds of the grid
bool isValid(int x, int y, int rows, int cols) { return (x >= 0 && x < rows && y >= 0 && y < cols); }

// Function to calculate the distance of each tile in the loop from the starting point 'S'
int calculateFarthestDistance(const vector<string>& grid) {
    int rows = grid.size();
    int cols = grid[0].size();

    // Define the directions: up, down, left, right
    vector<int> dx = {-1, 1, 0, 0};
    vector<int> dy = {0, 0, -1, 1};

    const unordered_map<char, pair<int, int>> dirToIndex = {{'|', {0, 1}},  {'-', {1, 0}},  {'L', {1, -1}},
                                                            {'J', {1, -1}}, {'7', {-1, 1}}, {'F', {-1, 1}}};

    // Find the starting position 'S'
    Point start;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (grid[i][j] == 'S') {
                start.x = i;
                start.y = j;
                break;
            }
        }
    }

    queue<Point> q;
    q.push(start);

    unordered_map<int, unordered_map<int, int>> distance;

    // Perform BFS to calculate distances
    while (!q.empty()) {
        Point curr = q.front();
        q.pop();

        for (auto& direction : dirToIndex) {
            if (grid[curr.x][curr.y] != direction.first && grid[curr.x][curr.y] != 'S') {
                continue;
            }
            int dx = direction.second.first;
            int dy = direction.second.second;
            int nx = curr.x + dx;
            int ny = curr.y + dy;

            if (isValid(nx, ny, rows, cols) && grid[nx][ny] != '.' && distance[nx][ny] == 0) {
                distance[nx][ny] = distance[curr.x][curr.y] + 1;
                LOG(INFO) << OUT(nx) << OUT(ny) << OUT(distance[nx][ny]);
                q.push({nx, ny});
            }
        }
    }

    // Find the maximum distance from the starting point 'S'
    int maxDistance = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (distance[i][j] > maxDistance) {
                maxDistance = distance[i][j];
            }
        }
    }

    return maxDistance;
}

// Define the symbols for the pipes
const char VERTICAL = '|';
const char HORIZONTAL = '-';
const char NORTH_EAST = 'L';
const char NORTH_WEST = 'J';
const char SOUTH_WEST = '7';
const char SOUTH_EAST = 'F';
const char START = 'S';
const char VISITED = 'X';

// Define the directions as pairs of row and column offsets
const vector<pair<int, int>> DIRECTIONS = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

// Check if a tile is part of the loop
bool isPipe(char tile) {
    return tile == VERTICAL || tile == HORIZONTAL || tile == NORTH_EAST ||
           tile == NORTH_WEST || tile == SOUTH_WEST || tile == SOUTH_EAST ||
           tile == START;
}

// Check if a move from a tile to another is valid
bool isValidMove(char from, char to, int dir) {
    // dir is the index of the direction in the DIRECTIONS vector
    // 0 = north, 1 = south, 2 = west, 3 = east
    switch (from) {
        case VERTICAL:
            return dir == 0 || dir == 1;
        case HORIZONTAL:
            return dir == 2 || dir == 3;
        case NORTH_EAST:
            return dir == 0 || dir == 3;
        case NORTH_WEST:
            return dir == 0 || dir == 2;
        case SOUTH_WEST:
            return dir == 1 || dir == 2;
        case SOUTH_EAST:
            return dir == 1 || dir == 3;
        case START:
            return true;
        default:
            return false;
    }
}

// Find the longest path from a given position
int findLongestPath(vector<string>& maze, int row, int col, int steps) {
    // Mark the current position as visited
    char tile = maze[row][col];
    maze[row][col] = VISITED;

    // Initialize the maximum steps to the current steps
    int maxSteps = steps;

    // Try all four directions
    for (int i = 0; i < 4; i++) {
        // Get the next position
        int nextRow = row + DIRECTIONS[i].first;
        int nextCol = col + DIRECTIONS[i].second;

        // Check if the next position is valid and part of the loop
        if (nextRow >= 0 && nextRow < maze.size() && nextCol >= 0 &&
            nextCol < maze[0].size() && isPipe(maze[nextRow][nextCol]) &&
            isValidMove(tile, maze[nextRow][nextCol], i)) {
            // Recursively find the longest path from the next position
            int nextSteps = findLongestPath(maze, nextRow, nextCol, steps + 1);

            // Update the maximum steps
            maxSteps = max(maxSteps, nextSteps);
        }
    }

    // Restore the original tile
    maze[row][col] = tile;

    // Return the maximum steps
    return maxSteps;
}

// Find the longest path in the maze
int solveMaze(vector<string>& maze) {
    // Find the starting position
    int startRow = -1;
    int startCol = -1;
    for (int i = 0; i < maze.size(); i++) {
        for (int j = 0; j < maze[0].size(); j++) {
            if (maze[i][j] == START) {
                startRow = i;
                startCol = j;
                break;
            }
        }
        if (startRow != -1) break;
    }

    // Return the longest path from the starting position
    return findLongestPath(maze, startRow, startCol, 0);
}

void first() {
    auto input = readInputLines();
    int result = solveMaze(input);
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
