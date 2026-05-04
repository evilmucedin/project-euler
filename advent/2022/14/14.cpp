#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

using Point = pair<int, int>;
unordered_set<Point> readRocks() {
    const auto input = readInputLines();
    unordered_set<Point> rocks;

    for (auto& s : input) {
        const auto ints = parseIntegers(replaceAll(s, "->", "  "));
        cerr << ints << endl;
        if (ints.empty()) {
            break;
        }
        Point prev = make_pair(ints[0], ints[1]);
        int i = 2;
        while (i < ints.size()) {
            Point next = make_pair(ints[i], ints[i + 1]);
            if (prev.first == next.first) {
                if (prev.second < next.second) {
                    for (int j = prev.second; j <= next.second; ++j) {
                        rocks.emplace(make_pair(prev.first, j));
                    }
                } else {
                    for (int j = next.second; j <= prev.second; ++j) {
                        rocks.emplace(make_pair(prev.first, j));
                    }
                }
            } else {
                if (prev.first < next.first) {
                    for (int j = prev.first; j <= next.first; ++j) {
                        rocks.emplace(make_pair(j, prev.second));
                    }
                } else {
                    for (int j = next.first; j <= prev.first; ++j) {
                        rocks.emplace(make_pair(j, prev.second));
                    }
                }
            }
            prev = next;
            i += 2;
        }
    }

    cerr << vector<Point>(ITRANGE(rocks)) << endl;
    return rocks;
}

void first() {
    auto rocks = readRocks();
    int result = 0;
    while (true) {
        Point s(500, 0);
        while (s.second < 1000) {
            auto can = [&](Point& p, int dx, int dy) {
                Point n = p;
                n.first += dx;
                n.second += dy;
                if (rocks.count(n) == 0) {
                    p = n;
                    return true;
                }
                return false;
            };

            if (!can(s, 0, 1)) {
                if (!can(s, -1, 1)) {
                    if (!can(s, 1, 1)) {
                        break;
                    }
                }
            }
        }
        if (s.second >= 1000) {
            break;
        }
        ++result;
        cerr << s << endl;
        rocks.emplace(s);
    }
    cout << result << endl;
}

void second() {
    auto rocks = readRocks();

    int maxY = 0;
    for (const auto& p : rocks) {
        maxY = max(maxY, p.second);
    }

    const int floor = maxY + 2;
    cerr << "Floor: " << floor << endl;

    int result = 0;
    while (rocks.count(make_pair(500, 0)) == 0) {
        Point s(500, 0);
        while (s.second < floor) {
            auto can = [&](Point& p, int dx, int dy) {
                Point n = p;
                n.first += dx;
                n.second += dy;

                if (n.second == floor) {
                    return false;
                }

                if (rocks.count(n) == 0) {
                    p = n;
                    return true;
                }
                return false;
            };

            if (!can(s, 0, 1)) {
                if (!can(s, -1, 1)) {
                    if (!can(s, 1, 1)) {
                        break;
                    }
                }
            }
        }
        ++result;
        cerr << s << endl;
        rocks.emplace(s);
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

