#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

struct Point {
    i64 x;
    i64 y;

    Point() {
    }

    Point(i64 x, i64 y) : x(x), y(y) {}
};

struct Line {
    i64 a;
    i64 b;
    i64 c;

    bool on(const Point& p) const {
        return v(p) == 0;
    }

    i64 v(const Point& p) const {
        return a*p.x + b*p.y + c;
    }
};

struct Interval {
    Line l;
    Point start;
    Point finish;

    Interval(Line l, Point start, Point finish) : l(l), start(start), finish(finish) {}

    bool on(const Point& p) const {
        if (l.on(p)) {
            if ((p.x - start.x)*(p.x - finish.x) > 0) {
                return false;
            }
            if ((p.y - start.y)*(p.y - finish.y) > 0) {
                return false;
            }
            return true;
        }
        return false;
    }
};

Line pointsToLine(const Point& a, const Point& b) {
    Line result;
    result.a = a.y - b.y;
    result.b = b.x - a.x;
    result.c = -result.a * a.x - result.b * a.y;
    return result;
}

void first() {
    auto strings = readInputLines();
    for (auto& s : strings) {
        s = replaceAll(s, "->", " ");
        s = replaceAll(s, ",", " ");
        // cerr << s << endl;
    }

    vector<Interval> ints;
    i64 minX = 100000000;
    i64 maxX = -100000000;
    i64 minY = 100000000;
    i64 maxY = -100000000;
    for (const auto& s : strings) {
        stringstream ss(s);
        Point a;
        Point b;
        ss >> a.x >> a.y >> b.x >> b.y;

        minX = min(minX, a.x);
        maxX = max(maxX, a.x);
        minY = min(minY, a.y);
        maxY = max(maxY, a.y);
        minX = min(minX, b.x);
        maxX = max(maxX, b.x);
        minY = min(minY, b.y);
        maxY = max(maxY, b.y);

        ints.emplace_back(pointsToLine(a, b), a, b);
    }

    size_t count = 0;
    for (i64 x = minX; x <= maxX; ++x) {
        for (i64 y = minY; y <= maxY; ++y) {
            size_t lineCount = 0;
            for (const auto& i: ints) {
                if (i.l.a == 0 || i.l.b == 0) {
                    if (i.on(Point(x, y))) {
                        ++lineCount;
                    }
                }
            }
            if (lineCount >= 2) {
                ++count;
            }
        }
    }

    cout << count << endl;
}

void second() {
    auto strings = readInputLines();
    for (auto& s : strings) {
        s = replaceAll(s, "->", " ");
        s = replaceAll(s, ",", " ");
        // cerr << s << endl;
    }

    vector<Interval> ints;
    i64 minX = 100000000;
    i64 maxX = -100000000;
    i64 minY = 100000000;
    i64 maxY = -100000000;
    for (const auto& s : strings) {
        stringstream ss(s);
        Point a;
        Point b;
        ss >> a.x >> a.y >> b.x >> b.y;

        minX = min(minX, a.x);
        maxX = max(maxX, a.x);
        minY = min(minY, a.y);
        maxY = max(maxY, a.y);
        minX = min(minX, b.x);
        maxX = max(maxX, b.x);
        minY = min(minY, b.y);
        maxY = max(maxY, b.y);

        ints.emplace_back(pointsToLine(a, b), a, b);
    }

    size_t count = 0;
    for (i64 x = minX; x <= maxX; ++x) {
        for (i64 y = minY; y <= maxY; ++y) {
            size_t lineCount = 0;
            for (const auto& i: ints) {
                if (i.l.a == 0 || i.l.b == 0 || (abs(i.l.a) == abs(i.l.b))) {
                    if (i.on(Point(x, y))) {
                        ++lineCount;
                    }
                }
            }
            if (lineCount >= 2) {
                ++count;
            }
        }
    }

    cout << count << endl;
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

