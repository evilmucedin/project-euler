#include <glog/logging.h>
#include <tqdm/tqdm.h>
#include "gflags/gflags.h"

#include <lib/init.h>
#include <lib/header.h>
#include <lib/thread-pool/threadPool.h>
#include <lib/io/stream.h>
#include <lib/string.h>
#include <lib/exception.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

static const int LIMIT = 70000;

DEFINE_int32(x1min, -LIMIT, "min x1");
DEFINE_int32(x1max, LIMIT, "max x1");

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    static const string STORAGE = "found.txt";

    using T = long double;
    T s = 0;
    using Point = pair<i64, i64>;
    using Triangle = vector<Point>;
    std::unordered_set<std::vector<Point>> l;

    auto points2p = [](const Triangle& t) -> tuple<T, T, T, T> {
        i64 d1 = sqr(t[0].first - t[1].first);
        i64 d2 = sqr(t[0].second - t[1].second);
        i64 d3 = sqr(t[0].first - t[2].first);
        i64 d4 = sqr(t[0].second - t[2].second);
        i64 d5 = sqr(t[1].first - t[2].first);
        i64 d6 = sqr(t[1].second - t[2].second);

        T a = std::sqrt(d1 + d2);
        T b = std::sqrt(d3 + d4);
        T c = std::sqrt(d5 + d6);

        T p = a + b + c;
        return {p, a, b, c};
    };

    auto isGood = [](const Triangle& t) {
        i64 o1 = sqr(t[0].first) + sqr(t[0].second);
        i64 o2 = sqr(t[1].first) + sqr(t[1].second);
        i64 o3 = sqr(t[2].first) + sqr(t[2].second);
        return o1 == o2 && o1 == o3;
    };

    {
        string line;
        auto fIn = openFileBufferedReader(STORAGE);
        while (fIn->readLine(line)) {
            if (line[0] == '#') {
                continue;
            }
            const auto parts = parseIntegers(line);
            ASSERTEQ(parts.size(), 6);
            const Triangle t{make_pair(parts[0], parts[1]), make_pair(parts[2], parts[3]), make_pair(parts[4], parts[5])};
            l.emplace(t);
            if (!isGood(t)) {
                THROW("Bad traingle in file " << t);
            }
        }
        for (const auto& t : l) {
            const auto [p, a, b, c] = points2p(t);
            s += p;
        }
    }

    tp::ThreadPool tp;

    std::mutex mtx;

    std::ofstream fFound;
    fFound.open(STORAGE, std::ios_base::app);

    auto f = [&](i64 x1) {
        LOG_EVERY_MS(INFO, 10000) << std::setprecision(10) << OUT(x1) << OUT(l.size()) << OUT(s);
        for (i64 x2 = -LIMIT; x2 < LIMIT; ++x2) {
            for (i64 y1 = -LIMIT; y1 < LIMIT; ++y1) {
                i64 k = x1 * x1 + y1 * y1 - x2 * x2;
                if (k < 0 || std::sqrt(k) != int(std::sqrt(k))) {
                    continue;
                }

                i64 y2 = std::sqrt(k);
                i64 x3 = 5 - x1 - x2;
                i64 y3 = -y1 - y2;

                Triangle t = {{x1, y1}, {x2, y2}, {x3, y3}};

                auto [p, a, b, c] = points2p(t);
                if (p > 100000) {
                    continue;
                }

                if (a + b <= c && a + c <= b && b + c <= a) {
                    continue;
                }

                if (isGood(t)) {
                    std::sort(std::begin(t), std::end(t));

                    {
                        std::unique_lock<std::mutex> lck(mtx);
                        bool found = l.count(t);

                        if (!found) {
                            fFound << t << endl;
                            l.emplace(t);
                            s += p;
                        }
                    }
                }
            }
        }
    };

    tp.runRange(f, FLAGS_x1min, FLAGS_x1max);

    std::cout << std::setprecision(10) << s << std::endl;

    return 0;
}
