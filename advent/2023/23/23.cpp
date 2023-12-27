#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

const int MAXN = 250;
const int dx[] = { 0, 1, 0, -1 }; // Directions: Down, Right, Up, Left
const int dy[] = { 1, 0, -1, 0 };

int R, C;
char grid[MAXN][MAXN];
bool visited[MAXN][MAXN];
int maxHikeLength = 0;

bool isValid(int x, int y) {
    return x >= 0 && x < R && y >= 0 && y < C && grid[x][y] != '#' && !visited[x][y];
}

void out() {
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            if (visited[i][j]) {
                cerr << 'O';
            } else {
                cerr << grid[i][j];
            }
        }
        cerr << endl;
    }
    cerr << endl;
}

void dfs(int x, int y, int length) {
    if (x + 1 == R) {
        out();
        maxHikeLength = max(maxHikeLength, length);
    }

    visited[x][y] = true;

    for (int dir = 0; dir < 4; ++dir) {
        int nx = x + dx[dir];
        int ny = y + dy[dir];

        if (isValid(nx, ny)) {
            if (grid[nx][ny] != '.' && grid[nx][ny] != '^') {
                int ndir = 0;
                switch (grid[nx][ny]) {
                    case '>': ndir = 1; break;
                    case 'v': ndir = 2; break;
                    case '<': ndir = 3; break;
                }
                if (dir != ndir) continue; // Must follow downhill direction
            }
            dfs(nx, ny, length + 1);
        }
    }

    visited[x][y] = false; // Backtrack
}

void first() {
    const auto input = readInputLines();

    R = input.size();
    C = input[0].size();

    int count = 0;
    for (int i = 0; i < R; ++i) {
        for (int j = 0; j < C; ++j) {
            grid[i][j] = input[i][j];
            if (grid[i][j] != '#') {
                ++count;
            }
        }
    }
    LOG(INFO) << OUT(count);

    // Find the starting position
    int startX = 0, startY = 0;
        for (int j = 0; j < C; ++j) {
            if (grid[0][j] == 'S') {
                startX = 0;
                startY = j;
                break;
            }
        }

    // Perform DFS to find the longest hike
    dfs(startX, startY, 1);

    cout << "The longest hike length is: " << maxHikeLength << " steps." << endl;

    cout << maxHikeLength << endl;
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

