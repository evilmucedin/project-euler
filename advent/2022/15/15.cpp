#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"
#include <lib/thread-pool/threadPool.h>

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

using Point = pair<int, int>;
using Points = vector<Point>;

int d(const Point& a, const Point& b) { return abs(a.first - b.first) + abs(a.second - b.second); }

tuple<Points, Points, int, int, int, int> input() {
    const auto input = readInputLines();

    Points sensors;
    Points beacons;

    int minX = 1234567890;
    int maxX = -1234567890;
    int minY = 1234567890;
    int maxY = -1234567890;
    for (const auto& s: input) {
        const auto parts = parseIntegers(s);
        // cerr << parts << endl;
        sensors.emplace_back(make_pair(parts[0], parts[1]));
        beacons.emplace_back(make_pair(parts[2], parts[3]));
        minX = min(minX, parts[0]);
        minX = min(minX, parts[2]);
        maxX = max(maxX, parts[0]);
        maxX = max(maxX, parts[2]);
        minY = min(minY, parts[1]);
        minY = min(minY, parts[3]);
        maxY = max(maxY, parts[1]);
        maxY = max(maxY, parts[3]);
    }
    cerr << "[" << minX << ", " << maxX << "]" << endl;
    sortAndUnique(beacons);
    return {sensors, beacons, minX, maxX, minY, maxY};
}

void first() {
    auto [sensors, beacons, minX, maxX, minY, maxY] = input();
    unordered_set<Point> sBeacons;
    sBeacons.insert(beacons.begin(), beacons.end());

    vector<int> minDs(sensors.size());
    for (int i = 0; i < sensors.size(); ++i) {
        int minD = 1234567890;
        for (const auto& b : beacons) {
            minD = min(minD, d(b, sensors[i]));
        }
        minDs[i] = minD;
    }

    int result = 0;
    // const int badY = 10;
    const int badY = 2000000;
    for (int badX = minX - 600000; badX < maxX + 600000; ++badX) {
        Point bad(badX, badY);
        if (sBeacons.count(bad)) {
            continue;
        }
        bool badF = false;
        for (int i = 0; i < sensors.size(); ++i) {
            const int dd = d(sensors[i], bad);
            if (minDs[i] >= dd) {
                badF = true;
                break;
            }
            // cerr << bad << " " << minD << " " << d(s, bad) << endl;
            if (badF) {
                break;
            }
        }
        if (badF) {
            ++result;
        }
    }

    cout << result << endl;
}

Points unite(Points& intervals) {
    Points result;
    sort(intervals.begin(), intervals.end());
    Point next = intervals[0];
    for (int i = 1; i < intervals.size(); ++i) {
        if (next.first <= intervals[i].first && intervals[i].first <= next.second) {
            next.second = intervals[i].second;
        } else {
            result.emplace_back(next);
            next = intervals[i];
        }
    }
    result.emplace_back(next);
    return result;
}

void second() {
    auto inp = input();
    const Points sensors = std::get<0>(inp);
    const Points beacons = std::get<1>(inp);
    int minY = std::get<4>(inp);
    int maxY = std::get<5>(inp);
    // auto [sensors, beacons, minX, maxX, minY, maxY] = input();

    vector<int> minDs(sensors.size());
    for (int i = 0; i < sensors.size(); ++i) {
        int minD = 1234567890;
        for (const auto& b : beacons) {
            minD = min(minD, d(b, sensors[i]));
        }
        minDs[i] = minD;
    }
    cerr << minDs << endl;

    auto checkXY = [&](int x, int y) {
        if (x < 0) {
            return false;
        }
        if (y < 0) {
            return false;
        }
        if (x > 4000000) {
            return false;
        }
        if (y > 4000000) {
            return false;
        }
        Point bad(x, y);
        int i = 0;
        while (i < sensors.size() && (minDs[i] < d(bad, sensors[i]))) {
            ++i;
        }
        if (i == sensors.size()) {
            cout << x << " " << y << " " << 4000000 * (long long)x + (long long)y << endl;
            return true;
        }
        return false;
    };

    const int R = 100;

    auto searchRX = [&](const Point& p) {
        for (int x = p.first - R; x <= p.first + R; ++x) {
            // LOG_EVERY_MS(INFO, 1000) << x;
            // for (int y = p.second - R; y <= p.second + R; ++y) {
                if (checkXY(x, p.second)) {
                    return true;
                }
            // }
        }
        return false;
    };

    for (int y = minY; y <= maxY; ++y) {
        LOG_EVERY_MS(INFO, 1000) << y;
        vector<Point> intervals;
        for (int i = 0; i < sensors.size(); ++i) {
            if (abs(sensors[i].second - y) <= minDs[i]) {
                int dd = minDs[i] - abs(sensors[i].second - y);
                intervals.emplace_back(make_pair(sensors[i].first - dd, sensors[i].first + dd));
            }
        }
        vector<Point> u = unite(intervals);
        // cerr << y << " " << u << endl;
        for (const auto& in : u) {
            if (searchRX(make_pair(in.first, y))) {
                cerr << u << endl;
            }
            if (searchRX(make_pair(in.second, y))) {
                cerr << u << endl;
            }
        }
    }
}

void second_() {
    auto inp = input();
    const Points sensors = std::get<0>(inp);
    const Points beacons = std::get<1>(inp);

    vector<int> minDs(sensors.size());
    for (int i = 0; i < sensors.size(); ++i) {
        int minD = 1234567890;
        for (const auto& b : beacons) {
            minD = min(minD, d(b, sensors[i]));
        }
        minDs[i] = minD;
    }
    cerr << minDs << endl;

    auto errXY = [&](int x, int y) {
        if (x < 0) {
            return 1234567890;
        }
        if (y < 0) {
            return 1234567890;
        }
        if (x > 4000000) {
            return 1234567890;
        }
        if (y > 4000000) {
            return 1234567890;
        }
        Point bad(x, y);
        int result = 0;
        for (int i = 0; i < sensors.size(); ++i) {
            int dd = d(bad, sensors[i]);
            if (minDs[i] >= dd) {
                result += minDs[i] - dd + 1;
            }
        }
        return result;
    };

    auto checkXY = [&](int x, int y) {
        if (x < 0) {
            return;
        }
        if (y < 0) {
            return;
        }
        if (x > 4000000) {
            return;
        }
        if (y > 4000000) {
            return;
        }
        Point bad(x, y);
        int i = 0;
        while (i < sensors.size() && (minDs[i] < d(bad, sensors[i]))) {
            ++i;
        }
        if (i == sensors.size()) {
            cout << x << " " << y << " " << 4000000 * (long long)x + (long long)y << endl;
        }
    };

    const int R = 10000;

    auto searchR = [&](const Point& p) {
        for (int x = p.first - R; x <= p.first + R; ++x) {
            // LOG_EVERY_MS(INFO, 1000) << x;
            for (int y = p.second - R; y <= p.second + R; ++y) {
                checkXY(x, y);
            }
        }
    };

    for (int it2 = 0; it2 < 1000; ++it2) {
        int x = rand() % 4000000;
        int y = rand() % 4000000;
        int err = errXY(x, y);
        for (int it = 0; it < 1000000; ++it) {
            LOG_EVERY_MS(INFO, 1000) << x << " " << y << " " << err;
            checkXY(x, y);
            static const int DIRS[] = {-1, 0,    1,   0,     0,    1, 0,   -1, -1434, 3434, 3434, 3,
                                       34, 3434, -56, -3456, -100, 0, 100, 0,  0,     100,  0,    -100};
            for (int i = 0; i < 12; ++i) {
                int xn = x + DIRS[2 * i];
                int yn = y + DIRS[2 * i + 1];
                int errN = errXY(xn, yn);
                if (errN < err) {
                    err = errN;
                    x = xn;
                    y = yn;
                    if (err < 10) {
                        searchR(make_pair(x, y));
                    }
                }
            }
        }
        checkXY(x, y);
    }

    auto f = [&](int i) {
        for (int x = beacons[i].first - R; x <= beacons[i].first + R; ++x) {
            // LOG_EVERY_MS(INFO, 1000) << x;
            for (int y = beacons[i].second - R; y <= beacons[i].second + R; ++y) {
                checkXY(x, y);
            }
        }
    };

    tp::ThreadPool tp;
    tp.runRange(f, 0, (int)beacons.size());

    /*
    minX = 2000000 - R;
    maxX = 2000000 + R;
    minY = 2000000 - R;
    maxY = 2000000 + R;
    for (int x = max(0, minX); x <= min(maxX, 4000000); ++x) {
        LOG_EVERY_MS(INFO, 1000) << x;
        for (int y = max(0, minY); y <= min(maxY, 4000000); ++y) {
            checkXY(x, y);
        }
    }
    */
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

