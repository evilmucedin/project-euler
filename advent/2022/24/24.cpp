#include <set>
#include <queue>

#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    const int n = input.size();
    const int m = input[0].size();

    using Field = vector<vector<vector<char>>>;

    const Field empty(n, vector<vector<char>>(m));

    vector<Field> fields;
    fields.emplace_back(empty);
    auto& f = fields[0];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            f[i][j].emplace_back(input[i][j]);
        }
    }
    f[0][1].clear();
    f[n - 1][m - 2].clear();

    auto wrapX = [&](int x) {
        if (x >= 1 && x + 1 < n) {
            return x;
        }
        return 1 + ((x - 1 + n - 2) % (n - 2));
    };
    auto wrapY = [&](int y) {
        if (y >= 1 && y + 1 < m) {
            return y;
        }
        return 1 + ((y - 1 + m - 2) % (m - 2));
    };

    auto print = [&](const Field& f) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (f[i][j].size() == 1) {
                    cerr << f[i][j][0];
                } else if (f[i][j].size() == 0) {
                    cerr << '.';
                } else {
                    cerr << f[i][j].size();
                }
            }
            cerr << endl;
        }
        cerr << endl;
    };

    for (int i = 0; i < 1000; ++i) {
        const Field& now = fields.back();
        if (i < 20) {
            cerr << "t=" << i << endl;
            print(now);
        }
        Field next = now;
        for (int j = 1; j + 1 < n; ++j) {
            for (int k = 1; k + 1 < m; ++k) {
                next[j][k].clear();
            }
        }
        for (int j = 1; j + 1 < n; ++j) {
            for (int k = 1; k + 1 < m; ++k) {
                for (char ch: now[j][k]) {
                    if (ch == '>') {
                        next[j][wrapY(k + 1)].emplace_back(ch);
                    } else if (ch == '<') {
                        next[j][wrapY(k - 1)].emplace_back(ch);
                    } else if (ch == 'v') {
                        next[wrapX(j + 1)][k].emplace_back(ch);
                    } else if (ch == '^') {
                        next[wrapX(j - 1)][k].emplace_back(ch);
                    }
                }
            }
        }
        fields.emplace_back(next);
    }

    struct Point {
        int t;
        int x;
        int y;

        bool operator<(const Point& p) const {
            if (t != p.t) {
                return t < p.t;
            }
            if (x != p.x) {
                return x < p.x;
            }
            if (y != p.y) {
                return y < p.y;
            }
            return false;
        }
    };
    queue<Point> q;
    set<Point> visited;
    q.emplace(Point{0, 0, 1});
    int result = 0;
    cerr << "{" << n << ", " << m << "}" << endl;
    while (!q.empty()) {
        const Point now = q.front();

        cerr << now.t << " " << now.x << " " << now.y << endl;

        q.pop();

        if ((now.x + 1 == n) && (now.y + 2 == m)) {
            cerr << "Found " << now.t << " " << now.x << " " << now.y << " " << n << " " << m << endl;
            result = now.t;
            break;
        }

        static const int DIRS[] = {1, 0, -1, 0, 0, 1, 0, -1, 0, 0};
        for (int i = 0; i < 5; ++i) {
            Point next = now;
            ++next.t;
            next.x = now.x + DIRS[2*i];
            next.y = now.y + DIRS[2*i + 1];
            if (next.x >= 0 && next.x < n && next.y >= 0 && next.y < m) {
                if (input[next.x][next.y] == '#') {
                    continue;
                }
            } else {
                continue;
            }
            if ((next.x + 1 != n || next.y + 2 != m) && (next.x != 0 || next.y != 1)) {
                next.x = wrapX(next.x);
                next.y = wrapY(next.y);
            }
            if (!fields[next.t][next.x][next.y].empty()) {
                continue;
            }
            if (!visited.count(next)) {
                visited.emplace(next);
                q.emplace(next);
            }
        }
    }

    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    const int n = input.size();
    const int m = input[0].size();

    using Field = vector<vector<vector<char>>>;

    const Field empty(n, vector<vector<char>>(m));

    vector<Field> fields;
    fields.emplace_back(empty);
    auto& f = fields[0];
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            f[i][j].emplace_back(input[i][j]);
        }
    }
    f[0][1].clear();
    f[n - 1][m - 2].clear();

    auto wrapX = [&](int x) {
        if (x >= 1 && x + 1 < n) {
            return x;
        }
        return 1 + ((x - 1 + n - 2) % (n - 2));
    };
    auto wrapY = [&](int y) {
        if (y >= 1 && y + 1 < m) {
            return y;
        }
        return 1 + ((y - 1 + m - 2) % (m - 2));
    };

    auto print = [&](const Field& f) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (f[i][j].size() == 1) {
                    cerr << f[i][j][0];
                } else if (f[i][j].size() == 0) {
                    cerr << '.';
                } else {
                    cerr << f[i][j].size();
                }
            }
            cerr << endl;
        }
        cerr << endl;
    };

    for (int i = 0; i < 1000; ++i) {
        const Field& now = fields.back();
        if (i < 20) {
            cerr << "t=" << i << endl;
            print(now);
        }
        Field next = now;
        for (int j = 1; j + 1 < n; ++j) {
            for (int k = 1; k + 1 < m; ++k) {
                next[j][k].clear();
            }
        }
        for (int j = 1; j + 1 < n; ++j) {
            for (int k = 1; k + 1 < m; ++k) {
                for (char ch: now[j][k]) {
                    if (ch == '>') {
                        next[j][wrapY(k + 1)].emplace_back(ch);
                    } else if (ch == '<') {
                        next[j][wrapY(k - 1)].emplace_back(ch);
                    } else if (ch == 'v') {
                        next[wrapX(j + 1)][k].emplace_back(ch);
                    } else if (ch == '^') {
                        next[wrapX(j - 1)][k].emplace_back(ch);
                    }
                }
            }
        }
        fields.emplace_back(next);
    }

    struct Point {
        int t;
        int x;
        int y;

        bool operator<(const Point& p) const {
            if (t != p.t) {
                return t < p.t;
            }
            if (x != p.x) {
                return x < p.x;
            }
            if (y != p.y) {
                return y < p.y;
            }
            return false;
        }
    };

    auto dfs = [&](int startT, int startX, int startY, int destX, int destY) {
        queue<Point> q;
        set<Point> visited;
        q.emplace(Point{startT, startX, startY});
        cerr << "{" << n << ", " << m << "}" << endl;
        while (!q.empty()) {
            const Point now = q.front();

            cerr << now.t << " " << now.x << " " << now.y << endl;

            q.pop();

            if ((now.x == destX) && (now.y == destY)) {
                cerr << "Found " << now.t << " " << now.x << " " << now.y << " " << n << " " << m << endl;
                return now.t;
            }

            static const int DIRS[] = {1, 0, -1, 0, 0, 1, 0, -1, 0, 0};
            for (int i = 0; i < 5; ++i) {
                Point next = now;
                ++next.t;
                next.x = now.x + DIRS[2 * i];
                next.y = now.y + DIRS[2 * i + 1];
                if (next.x >= 0 && next.x < n && next.y >= 0 && next.y < m) {
                    if (input[next.x][next.y] == '#') {
                        continue;
                    }
                } else {
                    continue;
                }
                if ((next.x + 1 != n || next.y + 2 != m) && (next.x != 0 || next.y != 1)) {
                    next.x = wrapX(next.x);
                    next.y = wrapY(next.y);
                }
                if (!fields[next.t][next.x][next.y].empty()) {
                    continue;
                }
                if (!visited.count(next)) {
                    visited.emplace(next);
                    q.emplace(next);
                }
            }
        }

        return 0;
    };

    int result1 = dfs(0, 0, 1, n - 1, m - 2);
    int result2 = dfs(result1 + 1, n - 1, m - 2, 0, 1);
    int result3 = dfs(result2 + 1, 0, 1, n - 1, m - 2);

    cout << result1 << " " << result2 << " " << result3 << endl;
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

