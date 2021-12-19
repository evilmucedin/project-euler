#include <set>

#include "gflags/gflags.h"
#include "glog/logging.h"

#include "lib/init.h"
#include "lib/string.h"
#include "lib/geometry.h"

#include "advent/lib/aoc.h"

DEFINE_int32(test, 1, "test number");

using Point = Point3<int>;
using Points = vector<Point>;

vector<Points> input() {
    const auto input = readInputLinesAll();
    vector<Points> result;
    for (const auto& line : input) {
        if (!line.empty()) {
            if (line[0] == '-' && line[1] == '-') {
                result.emplace_back(Points());
            } else {
                Point p;
                sscanf(line.c_str(), "%d,%d,%d", &p.x, &p.y, &p.z);
                result.back().emplace_back(std::move(p));
            }
        }
    }
    return result;
}

using Matrix = vector<vector<int>>;

struct Operator {
    vector<int> perm{0, 0, 0};
    vector<int> sign{0, 0, 0};
    Point move{0, 0, 0};

    Point apply(const Point& p) const {
        Point result;
        result.x = p[perm[0]];
        result.y = p[perm[1]];
        result.z = p[perm[2]];
        result.x *= sign[0];
        result.y *= sign[1];
        result.z *= sign[2];
        result += move;
        return result;
    }
};

template <class InputIterator1, class InputIterator2>
size_t setIntersectionSize(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    size_t result = 0;
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else {
            result++;
            ++first1;
            ++first2;
        }
    }
    return result;
}

bool findOperator(const set<Point>& s0, const vector<Point>& data0, const vector<Point> data, Operator& o) {
    size_t maxCount = 0;
    for (int perm1 = 0; perm1 < 3; ++perm1) {
        for (int perm2 = 0; perm2 < 3; ++perm2) {
            if (perm1 == perm2) {
                continue;
            }
            for (int perm3 = 0; perm3 < 3; ++perm3) {
                if (perm1 == perm3 || perm2 == perm3) {
                    continue;
                }
                for (int sign1 = -1; sign1 < 2; sign1 += 2) {
                    for (int sign2 = -1; sign2 < 2; sign2 += 2) {
                        for (int sign3 = -1; sign3 < 2; sign3 += 2) {
                            for (int i = 0; i < data0.size(); ++i) {
                                for (int j = 0; j < data.size(); ++j) {
                                    o.perm = {perm1, perm2, perm3};
                                    o.sign = {sign1, sign2, sign3};
                                    o.move = {0, 0, 0};
                                    const auto& p0 = data0[i];
                                    const auto& p = o.apply(data[j]);
                                    // cerr << o.move << endl;
                                    o.move = p0 - p;

                                    // cerr << p0 << ", " << data[j] << ", " << o.apply(data[j]) << endl;

                                    size_t count = 0;
                                    for (const auto& p : data) {
                                        if (s0.count(o.apply(p))) {
                                            ++count;
                                        }
                                    }
                                    // cerr << o.move << " " << count << endl;
                                    if (count > maxCount) {
                                        maxCount = count;
                                    }
                                    /*
                                    LOG_EVERY_MS(INFO, 100)
                                        << OUT(perm1) << OUT(perm2) << OUT(perm3) << OUT(sign1) << OUT(sign2)
                                        << OUT(sign3) << OUT(count); //    << OUT(move1) << OUT(move2) << OUT(move3);
                                    */
                                    if (count >= 12) {
                                        return true;
                                    }
                                }
                            }
                            /*
                            for (int move1 = -1000; move1 < 1000; ++move1) {
                                for (int move2 = -1000; move2 < 1000; ++move2) {
                                    for (int move3 = -1000; move3 < 1000; ++move3) {
                                    }
                                }
                            }
                            */
                        }
                    }
                }
            }
        }
    }

    return false;
}

int md(const Point& a, const Point& b) { return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z); }

void first() {
    const auto data = input();
    cerr << data.size() << "x" << data[0].size() << endl;

    set<Point> result;
    for (const auto& p : data[0]) {
        result.insert(p);
    }

    vector<bool> good(data.size());

    bool change = true;
    vector<Point> data0 = data[0];
    vector<Point> moves;
    moves.emplace_back(Point{0, 0, 0});
    while (change) {
        change = false;
        for (int i = 1; i < data.size(); ++i) {
            if (good[i]) {
                continue;
            }
            Operator o;
            if (findOperator(result, data0, data[i], o)) {
                good[i] = true;
                change = true;
                moves.emplace_back(o.move);
                for (const auto& p : data[i]) {
                    auto ps = o.apply(p);
                    if (!result.count(ps)) {
                        result.insert(ps);
                        data0.emplace_back(ps);
                    }
                }
            }
        }
    }

    int max = 0;
    for (const auto& a : moves) {
        for (const auto& b : moves) {
            auto d = md(a, b);
            if (d > max) {
                max = d;
            }
        }
    }

    cerr << good << endl;
    cout << result.size() << endl;
    cout << "mx: " << max << endl;
}

void second() { first(); }

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
