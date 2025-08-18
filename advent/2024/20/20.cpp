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
#include <algorithm>
#include <set>

using namespace std;

const vector<pair<int, int>> directions = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
const int MAX_N = 100;

struct State {
    int x, y, steps, cheats;
    State(int x, int y, int steps, int cheats) : x(x), y(y), steps(steps), cheats(cheats) {}
};

int bfs(vector<string>& maze, int startX, int startY, int endX, int endY, bool withCheat, set<int>& cheatTimes) {
    int rows = maze.size();
    int cols = maze[0].size();
    vector<vector<vector<int>>> dist(rows, vector<vector<int>>(cols, vector<int>(2, MAX_N)));
    queue<State> q;
    q.push(State(startX, startY, 0, 0));
    dist[startY][startX][0] = 0;

    while (!q.empty()) {
        auto [x, y, steps, cheats] = q.front();
        q.pop();

        if (x == endX && y == endY) {
            if (withCheat && cheats > 0) cheatTimes.insert(steps);
            return steps;
        }

        for (auto [dx, dy] : directions) {
            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && ny >= 0 && nx < cols && ny < rows) {
                // Move without cheating
                if (maze[ny][nx] == '.' || maze[ny][nx] == 'E') {
                    if (steps + 1 < dist[ny][nx][cheats]) {
                        dist[ny][nx][cheats] = steps + 1;
                        q.push(State(nx, ny, steps + 1, cheats));
                    }
                }
                // Move with cheating
                if (cheats == 0 && maze[ny][nx] == '#') {
                    if (steps + 1 < dist[ny][nx][cheats + 1]) {
                        dist[ny][nx][cheats + 1] = steps + 1;
                        q.push(State(nx, ny, steps + 1, cheats + 1));
                    }
                }
            }
        }
    }

    return MAX_N;
}

int main() {
    vector<string> maze = {
        "###############",
        "#...#...#.....#",
        "#.#.#.#.#.###.#",
        "#S#...#.#.#...#",
        "#######.#.#.###",
        "#######.#.#...#",
        "#######.#.###.#",
        "###..E#...#...#",
        "###.#######.###",
        "#...###...#...#",
        "#.#####.#.###.#",
        "#.#...#.#.#...#",
        "#.#.#.#.#.#.###",
        "#...#...#...###",
        "###############"
    };
    maze = readInputLines();

    int startX, startY, endX, endY;
    for (int y = 0; y < maze.size(); ++y) {
        for (int x = 0; x < maze[0].size(); ++x) {
            if (maze[y][x] == 'S') {
                startX = x;
                startY = y;
            } else if (maze[y][x] == 'E') {
                endX = x;
                endY = y;
            }
        }
    }

    set<int> cheatTimes;
    int fastestTimeWithoutCheat = bfs(maze, startX, startY, endX, endY, false, cheatTimes);
    int fastestTimeWithCheat = bfs(maze, startX, startY, endX, endY, true, cheatTimes);

    int numGoodCheats = 0;
    for (const int& time : cheatTimes) {
        if (fastestTimeWithoutCheat - time >= 100) {
            numGoodCheats++;
        }
    }

    cout << "Number of good cheats (saving at least 100 picoseconds): " << numGoodCheats << endl;

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
