#include <unordered_map>
#include <queue>

#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

bool allDots(const string& s) {
    for (char ch: s) {
        if (ch != '.') {
            return false;
        }
    }
    return true;
}

void first() {
    auto input = readInputLines();
    if (input.size()) {
        for (int i = 0; i < input.size(); ++i) {
            if (allDots(input[i])) {
                input.insert(input.begin() + i, input[i]);
                ++i;
            }
        }
        for (int i = 0; i < input[0].size(); ++i) {
            int j = 0;
            while (j < input.size() && input[j][i] == '.') {
                ++j;
            }
            if (j == input.size()) {
                for (j = 0; j < input.size(); ++j) {
                    input[j].insert(input[j].begin() + i, '.');
                }
                ++i;
            }
        }
    }

    const int n = input.size();
    const int m = input[0].size();

    vector<pair<int, int>> points;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (input[i][j] == '#') {
                points.emplace_back(make_pair(i, j));
            }
        }
    }

    long long result = 0;
    for (int i = 0; i < points.size(); ++i) {
        unordered_map<int, unordered_map<int, int>> d;
        queue<pair<int, int>> q;
        q.emplace(points[i]);
        d[points[i].first][points[i].second] = 0;

        while (!q.empty()) {
            auto tp = q.front();
            q.pop();

            static constexpr int DIRS[] = {-1, 0, 1, 0, 0, 1, 0, -1};
            for (int id = 0; id < 4; ++id) {
                int x = tp.first + DIRS[2 * id];
                int y = tp.second + DIRS[2 * id + 1];
                if (x >= 0 && x < n && y >= 0 && y < m) {
                    if (!d[x].count(y)) {
                        d[x][y] = d[tp.first][tp.second] + 1;
                        q.emplace(make_pair(x, y));
                    }
                }
            }
        }

        for (int j = i + 1; j < points.size(); ++j) {
            if (i != j) {
                result += d[points[j].first][points[j].second];
            }
        }
    }

    cout << result << endl;
}

void second() {
    auto input = readInputLines();
    if (input.size()) {
        for (int i = 0; i < input.size(); ++i) {
            if (allDots(input[i])) {
                input.insert(input.begin() + i, string(input[i].size(), ','));
                ++i;
            }
        }
        for (int i = 0; i < input[0].size(); ++i) {
            int j = 0;
            while (j < input.size() && input[j][i] == '.') {
                ++j;
            }
            if (j == input.size()) {
                for (j = 0; j < input.size(); ++j) {
                    input[j].insert(input[j].begin() + i, ',');
                }
                ++i;
            }
        }
    }

    const int n = input.size();
    const int m = input[0].size();

    vector<pair<int, int>> points;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (input[i][j] == '#') {
                points.emplace_back(make_pair(i, j));
            }
        }
    }

    long long result = 0;
    for (int i = 0; i < points.size(); ++i) {
        unordered_map<int, unordered_map<int, long long>> d;
        priority_queue<pair<long long, pair<int, int>>> q;
        q.emplace(make_pair(0, points[i]));
        d[points[i].first][points[i].second] = 0;

        while (!q.empty()) {
            auto tp = q.top().second;
            q.pop();

            static constexpr int DIRS[] = {-1, 0, 1, 0, 0, 1, 0, -1};
            for (int id = 0; id < 4; ++id) {
                int x = tp.first + DIRS[2 * id];
                int y = tp.second + DIRS[2 * id + 1];
                if (x >= 0 && x < n && y >= 0 && y < m) {
                    if (!d[x].count(y)) {
                        d[x][y] = d[tp.first][tp.second] + ((input[x][y] == '.') ? 1 : 99);
                        q.emplace(-d[x][y], make_pair(x, y));
                    }
                }
            }
        }

        for (int j = i + 1; j < points.size(); ++j) {
            if (i != j) {
                result += d[points[j].first][points[j].second];
            }
        }
    }

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

