#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;

    using Point = pair<int, int>;
    using Points = unordered_set<Point>;

    Points now;
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            if (input[i][j] == '#') {
                now.emplace(make_pair(j, i));
            }
        }
    }

    static const int DIRS[] = {0, -1, 0, 1, -1, 0, 1, 0};
    static const vector<vector<int>> CAND = {
        {0, -1, -1, -1, 1, -1}, {0, 1, -1, 1, 1, 1}, {-1, 0, -1, -1, -1, 1}, {1, 0, 1, -1, 1, 1}};
    int iDir = 0;

    for (int i = 0; i < 10; ++i) {
        Points next;

        unordered_map<Point, Point> dests;
        unordered_map<Point, int> dest2count;

        auto has = [&](const Point& p, int dx, int dy) { return now.count(make_pair(p.first + dx, p.second + dy)); };

        for (const auto& p: now) {
            int count = 0;
            for (int j = -1; j <= 1; ++j) {
                for (int k = -1; k <= 1; ++k) {
                    count += has(p, j, k);
                }
            }

            if (count == 1) {
                continue;
            }

            for (int j = 0; j < 4; ++j) {
                int d = (iDir + j) % 4;
                if (!has(p, CAND[d][0], CAND[d][1]) && !has(p, CAND[d][2], CAND[d][3]) && !has(p, CAND[d][4], CAND[d][5])) {
                    Point dest = p;
                    dest.first += DIRS[2 * d];
                    dest.second += DIRS[2 * d + 1];
                    dests[p] = dest;
                    ++dest2count[dest];
                    break;
                }
            }
        }

        for (const auto& p : now) {
            if (dests.count(p) && dest2count[dests[p]] == 1) {
                next.emplace(dests[p]);
            } else {
                next.emplace(p);
            }
        }

        iDir = (iDir + 1) % 4;

        now.swap(next);

        int minX = 100000;
        int maxX = -100000;
        int minY = 100000;
        int maxY = -100000;
        for (const auto& p : now) {
            minX = min(minX, p.first);
            maxX = max(maxX, p.first);
            minY = min(minY, p.second);
            maxY = max(maxY, p.second);
        }

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                cerr << (now.count(make_pair(x, y)) ? "#" : ".");
            }
            cerr << endl;
        }
        cerr << endl;
    }

    int minX = 100000;
    int maxX = -100000;
    int minY = 100000;
    int maxY = -100000;
    for (const auto& p: now) {
        minX = min(minX, p.first);
        maxX = max(maxX, p.first);
        minY = min(minY, p.second);
        maxY = max(maxY, p.second);
    }
    cerr << OUT(minX) << OUT(maxX) << OUT(minY) << OUT(maxY) << OUT(now.size()) << endl;

    result = (maxX - minX + 1)*(maxY - minY + 1) - now.size();

    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;

    using Point = pair<int, int>;
    using Points = unordered_set<Point>;

    Points now;
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            if (input[i][j] == '#') {
                now.emplace(make_pair(j, i));
            }
        }
    }

    static const int DIRS[] = {0, -1, 0, 1, -1, 0, 1, 0};
    static const vector<vector<int>> CAND = {
        {0, -1, -1, -1, 1, -1}, {0, 1, -1, 1, 1, 1}, {-1, 0, -1, -1, -1, 1}, {1, 0, 1, -1, 1, 1}};
    int iDir = 0;

    bool good = true;
    while (good) {
        good = false;

        Points next;

        unordered_map<Point, Point> dests;
        unordered_map<Point, int> dest2count;

        auto has = [&](const Point& p, int dx, int dy) { return now.count(make_pair(p.first + dx, p.second + dy)); };

        for (const auto& p: now) {
            int count = 0;
            for (int j = -1; j <= 1; ++j) {
                for (int k = -1; k <= 1; ++k) {
                    count += has(p, j, k);
                }
            }

            if (count == 1) {
                continue;
            }

            for (int j = 0; j < 4; ++j) {
                int d = (iDir + j) % 4;
                if (!has(p, CAND[d][0], CAND[d][1]) && !has(p, CAND[d][2], CAND[d][3]) && !has(p, CAND[d][4], CAND[d][5])) {
                    Point dest = p;
                    dest.first += DIRS[2 * d];
                    dest.second += DIRS[2 * d + 1];
                    dests[p] = dest;
                    ++dest2count[dest];
                    break;
                }
            }
        }

        for (const auto& p : now) {
            if (dests.count(p) && dest2count[dests[p]] == 1) {
                next.emplace(dests[p]);
                good = true;
            } else {
                next.emplace(p);
            }
        }

        iDir = (iDir + 1) % 4;

        now.swap(next);

        /*
        int minX = 100000;
        int maxX = -100000;
        int minY = 100000;
        int maxY = -100000;
        for (const auto& p : now) {
            minX = min(minX, p.first);
            maxX = max(maxX, p.first);
            minY = min(minY, p.second);
            maxY = max(maxY, p.second);
        }

        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                cerr << (now.count(make_pair(x, y)) ? "#" : ".");
            }
            cerr << endl;
        }
        cerr << endl;
        */
        ++result;
    }

    /*
    int minX = 100000;
    int maxX = -100000;
    int minY = 100000;
    int maxY = -100000;
    for (const auto& p: now) {
        minX = min(minX, p.first);
        maxX = max(maxX, p.first);
        minY = min(minY, p.second);
        maxY = max(maxY, p.second);
    }
    cerr << OUT(minX) << OUT(maxX) << OUT(minY) << OUT(maxY) << OUT(now.size()) << endl;

    result = (maxX - minX + 1)*(maxY - minY + 1) - now.size();
    */

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

