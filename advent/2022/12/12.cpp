#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

using Point = pair<int, int>;

int d(const vector<string>& input, const Point& s, const Point& e) {
    const int n = input.size();
    const int m = input[0].size();
    vector<vector<Point>> parent(n, vector<Point>(m));
    vector<vector<bool>> visited(n, vector<bool>(m, false));
    queue<Point> q;
    q.emplace(s);
    while (!q.empty()) {
        const auto top = q.front();
        q.pop();
        if (!visited[top.first][top.second]) {
            visited[top.first][top.second] = true;
            static constexpr int DIRS[] = {1, 0, -1, 0, 0, 1, 0, -1};
            for (int i = 0; i < 4; ++i) {
                Point nxt = top;
                nxt.first += DIRS[2*i];
                nxt.second += DIRS[2*i + 1];
                if (nxt.first >= 0 && nxt.first < n && nxt.second >= 0 && nxt.second < m) {
                    if (input[nxt.first][nxt.second] <= input[top.first][top.second] + 1) {
                        if (!visited[nxt.first][nxt.second]) {
                            parent[nxt.first][nxt.second] = top;
                            q.emplace(nxt);
                        }
                    }
                }
            }
        }
    }

    if (!visited[e.first][e.second]) {
        return 1234567890;
    }

    int result = 0;
    Point now = e;
    while (now != s) {
        now = parent[now.first][now.second];
        ++result;
    }
    return result;
}

pair<Point, Point> findSE(vector<string>& input) {
    const int n = input.size();
    const int m = input[0].size();
    Point s;
    Point e;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (input[i][j] == 'S') {
                s = make_pair(i, j);
                input[i][j] = 'a';
            }
            if (input[i][j] == 'E') {
                e = make_pair(i, j);
                input[i][j] = 'z';
            }
        }
    }
    return make_pair(s, e);
}

void first() {
    auto input = readInputLines();
    auto [s, e] = findSE(input);
    cout << d(input, s, e)<< endl;
}

void second() {
    auto input = readInputLines();
    const int n = input.size();
    const int m = input[0].size();
    auto [s, e] = findSE(input);

    int min = 1234567890;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (input[i][j] == 'a') {
                int res = d(input, make_pair(i, j), e);
                if (res < min) {
                    min = res;
                }
            }
        }
    }
    cout << min << endl;
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

