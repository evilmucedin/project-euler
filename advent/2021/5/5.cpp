#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/geometry.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

using Point = Point2<i64>;
using Line = Line2<i64>;
using Interval = Interval2<i64>;

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

        ints.emplace_back(Line::fromPoints(a, b), a, b);
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

        ints.emplace_back(Line::fromPoints(a, b), a, b);
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

