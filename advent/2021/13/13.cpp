#include <set>

#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

struct Point {
    int x;
    int y;

    Point() {
    }

    Point(int x, int y) : x(x), y(y) {
    }

    bool operator<(const Point& p) const {
        if (x != p.x) {
            return x < p.x;
        }
        return y < p.y;
    }

    bool operator==(const Point& p) const {
        return (x == p.x) && (y == p.y);
    }
};
using Points = vector<Point>;

Points mirrorX(const Points& ps, int x) {
    Points result;
    for (const auto& p: ps) {
        if (p.x <= x) {
            result.emplace_back(p);
        } else {
            result.emplace_back(x - (p.x - x), p.y);
        }
    }
    return result;
}

Points mirrorY(const Points& ps, int y) {
    Points result;
    for (const auto& p: ps) {
        if (p.y <= y) {
            result.emplace_back(p);
        } else {
            result.emplace_back(p.x, y - (p.y - y));
        }
    }
    return result;
}

void first() {
    const auto input = readInputLinesAll();

    Points ps;
    size_t iLine = 0;
    while (!input[iLine].empty()) {
        const auto parts = split(input[iLine], ',');
        ASSERTEQ(parts.size(), 2);
        ps.emplace_back(stoi(parts[0]), stoi(parts[1]));
        ++iLine;
    }
    ++iLine;

    cerr << iLine << endl << input[iLine] << endl;
    const auto parts = split(input[iLine], '=');
    const auto x = stoi(parts[1]);

    ps = mirrorX(ps, x);

    set<Point> sp(ps.begin(), ps.end());

    cout << sp.size() << endl;

}

void second() {
    const auto input = readInputLinesAll();

    Points ps;
    size_t iLine = 0;
    while (!input[iLine].empty()) {
        const auto parts = split(input[iLine], ',');
        ASSERTEQ(parts.size(), 2);
        ps.emplace_back(stoi(parts[0]), stoi(parts[1]));
        ++iLine;
    }
    ++iLine;

    while (iLine < input.size()) {
        cerr << iLine << endl << input[iLine] << endl;
        const auto parts = split(input[iLine], '=');
        if (parts[0][parts[0].size() - 1] == 'x') {
            const auto x = stoi(parts[1]);
            ps = mirrorX(ps, x);
        } else {
            const auto y = stoi(parts[1]);
            ps = mirrorY(ps, y);
        }
        ++iLine;

        set<Point> sp(ps.begin(), ps.end());
        cout << sp.size() << endl;
    }

    ofstream fOut("second.out");
    for (const auto& p: ps) {
        fOut << p.x << "\t" <<  p.y << endl;
    }
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

