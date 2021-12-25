#include <set>

#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

struct Point {
    char type;
    int x;
    int y;

    bool operator<(const Point& p) const {
        if (x != p.x) {
            return x < p.x;
        }
        return y < p.y;
    }

    bool operator==(const Point& p) const { return x == p.x && y == p.y; }
};

using Points = multiset<Point>;

template <bool TURN>
bool move(int n, int m, Points& ps) {
    Points newF;
    bool res = false;
    for (const auto& p : ps) {
        if (TURN) {
            if (p.type == '>') {
                Point next = p;
                next.y = (next.y + 1) % m;
                if (ps.find(next) == ps.end()) {
                    newF.insert(next);
                    res = true;
                } else {
                    newF.insert(p);
                }
            } else {
                newF.insert(p);
            }
        } else {
            if (p.type == 'v') {
                Point next = p;
                next.x = (next.x + 1) % n;
                if (ps.find(next) == ps.end()) {
                    newF.insert(next);
                    res = true;
                } else {
                    newF.insert(p);
                }
            } else {
                newF.insert(p);
            }
        }
    }
    ps.swap(newF);
    return res;
}

void print(int n, int m, const Points& ps) {
    StringVector f(n, string(m, '.'));
    for (const auto& p : ps) {
        f[p.x][p.y] = p.type;
    }
    for (const auto& s : f) {
        cerr << s << endl;
    }
    cerr << endl;
}

void first() {
    auto input = readInputLines();

    Points ps;
    int n = input.size();
    int m = input[0].size();

    REP(i, n) {
        REP(j, m) {
            if (input[i][j] != '.') {
                Point p;
                p.type = input[i][j];
                p.x = i;
                p.y = j;
                ps.insert(p);
            }
        }
    }

    int steps = 0;
    // print(n, m, ps);
    while (true) {
        // print(n, m, ps);
        bool res1 = move<true>(n, m, ps);
        bool res2 = move<false>(n, m, ps);
        if (!res1 && !res2) {
            break;
        }
        ++steps;
    }

    cout << steps + 1 << endl;
}

void second() {
    const auto input = readInputLines();
    const int n = input.size();
    const int m = input[0].size();

    int steps = 0;
    auto f = input;
    while (true) {
        bool cont = false;
        auto newF = f;
        REP(i, input.size()) {
            REP(j, input[i].size()) {
                if (f[i][j] == '>' && f[i][(j + 1) % m] == '.') {
                    swap(newF[i][(j + 1) % m], newF[i][j]);
                    cont = true;
                }
            }
        }
        f = newF;
        REP(i, input.size()) {
            REP(j, input[i].size()) {
                if (f[i][j] == 'v' && f[(i + 1) % n][j] == '.') {
                    swap(newF[(i + 1) % n][j], newF[i][j]);
                    cont = true;
                }
            }
        }
        f = newF;
        ++steps;
        if (!cont) {
            break;
        }
    }

    cout << "1: " << steps << endl;
    string s(50, '*');
    cout << "2: " << s << endl;
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
