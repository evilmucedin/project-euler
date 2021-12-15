#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

struct Vertex {
    int risk;
    int min{1000000000};
};

using Line = vector<Vertex>;
using Matrix = vector<Line>;

void first() {
    const auto input = readInputLines();

    Matrix m(input.size(), Line(input[0].size()));

    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[0].size(); ++j) {
            m[i][j].risk = input[i][j] - '0';
        }
    }

    m[0][0].min = 0;

    queue<pair<int, int>> q;
    q.emplace(0, 0);

    while (!q.empty()) {
        const auto now = q.front();
        q.pop();
        static const int D[] = {-1, 0, 1, 0, 0, -1, 0, 1};
        for (int d = 0; d < 4; ++d) {
            const int x = now.first + D[2 * d];
            const int y = now.second + D[2 * d + 1];
            if (x >= 0 && x < m.size()) {
                if (y >= 0 && y < m[x].size()) {
                    auto next = m[now.first][now.second].min + m[x][y].risk;
                    if (next < m[x][y].min) {
                        m[x][y].min = next;
                        q.emplace(x, y);
                    }
                }
            }
        }
    }

    cout << m.back().back().min << endl;
}

void second() {
    const auto input = readInputLines();

    Matrix m(5*input.size(), Line(5*input[0].size()));

    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[0].size(); ++j) {
            m[i][j].risk = input[i][j] - '0';
        }
    }

    const auto in = input.size();
    const auto im = input[0].size();
    for (int i = 0; i < m.size(); ++i) {
        for (int j = 0; j < m[i].size(); ++j) {
            m[i][j].risk = ((m[i % in][j % im].risk + (i / in) + (j / im) - 1) % 9) + 1;
        }
    }

    m[0][0].min = 0;

    queue<pair<int, int>> q;
    q.emplace(0, 0);

    size_t steps = 0;
    while (!q.empty()) {
        const auto now = q.front();
        q.pop();
        static const int D[] = {-1, 0, 1, 0, 0, -1, 0, 1};
        for (int d = 0; d < 4; ++d) {
            const int x = now.first + D[2 * d];
            const int y = now.second + D[2 * d + 1];
            if (x >= 0 && x < m.size()) {
                if (y >= 0 && y < m[x].size()) {
                    auto next = m[now.first][now.second].min + m[x][y].risk;
                    if (next < m[x][y].min) {
                        m[x][y].min = next;
                        q.emplace(x, y);
                    }
                }
            }
        }
        ++steps;
    }
    cerr << steps << " " << in * im << endl;

    cout << m.back().back().min << endl;
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
