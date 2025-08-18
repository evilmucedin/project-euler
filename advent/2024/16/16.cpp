#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

#include <iostream>
#include <vector>
#include <queue>
#include <tuple>
#include <unordered_map>

using namespace std;

// Directions: 0 = East, 1 = South, 2 = West, 3 = North
const int dx[4] = {1, 0, -1, 0};
const int dy[4] = {0, 1, 0, -1};

// Function to find the minimum score using BFS with priority queue
int minScore(vector<string>& maze) {
    int rows = maze.size();
    int cols = maze[0].size();
    int start_x, start_y, end_x, end_y;

    // Find the start (S) and end (E) positions
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (maze[i][j] == 'S') {
                start_x = j;
                start_y = i;
            } else if (maze[i][j] == 'E') {
                end_x = j;
                end_y = i;
            }
        }
    }

    // Priority queue to store the states (cost, x, y, direction)
    priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, greater<tuple<int, int, int, int>>> pq;
    pq.push({0, start_x, start_y, 0}); // Start facing East with 0 cost

    // Distance map to store the minimum cost to reach a position with a certain direction
    unordered_map<int, unordered_map<int, unordered_map<int, int>>> dist;
    dist[start_y][start_x][0] = 0;

    while (!pq.empty()) {
        auto [cost, x, y, dir] = pq.top();
        pq.pop();

        // If reached the end
        if (x == end_x && y == end_y) return cost;

        // Try moving forward
        int nx = x + dx[dir];
        int ny = y + dy[dir];
        if (nx >= 0 && nx < cols && ny >= 0 && ny < rows && maze[ny][nx] != '#') {
            if (dist[ny][nx].count(dir) == 0 || cost + 1 < dist[ny][nx][dir]) {
                dist[ny][nx][dir] = cost + 1;
                pq.push({cost + 1, nx, ny, dir});
            }
        }

        // Try rotating clockwise
        int ndir = (dir + 1) % 4;
        if (dist[y][x].count(ndir) == 0 || cost + 1000 < dist[y][x][ndir]) {
            dist[y][x][ndir] = cost + 1000;
            pq.push({cost + 1000, x, y, ndir});
        }

        // Try rotating counterclockwise
        ndir = (dir + 3) % 4;
        if (dist[y][x].count(ndir) == 0 || cost + 1000 < dist[y][x][ndir]) {
            dist[y][x][ndir] = cost + 1000;
            pq.push({cost + 1000, x, y, ndir});
        }
    }

    return -1; // If no path found
}

int main() {
    vector<string> maze = {
        "###############",
        "#.......#....E#",
        "#.#.###.#.###.#",
        "#.....#.#...#.#",
        "#.###.#####.#.#",
        "#.#.#.......#.#",
        "#.#.#####.###.#",
        "#...........#.#",
        "###.#.#####.#.#",
        "#...#.....#.#.#",
        "#.#.#.###.#.#.#",
        "#.....#...#.#.#",
        "#.###.#.#.#.#.#",
        "#S..#.....#...#",
        "###############"
    };
    maze = readInputLines();

    int result = minScore(maze);
    cout << "The minimum score to navigate the maze is: " << result << endl;

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
