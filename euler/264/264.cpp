#include <glog/logging.h>
#include <tqdm/tqdm.h>
#include "gflags/gflags.h"

#include <lib/init.h>
#include <lib/header.h>
#include <lib/thread-pool/threadPool.h>
#include <lib/io/stream.h>
#include <lib/string.h>

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
    using Point = pair<int, int>;
    std::unordered_set<std::vector<Point>> l;

    auto points2p = [](int x1, int y1, int x2, int y2, int x3, int y3) -> tuple<T, T, T, T> {
        int d1 = (x1 - x2) * (x1 - x2);
        int d2 = (y1 - y2) * (y1 - y2);
        int d3 = (x1 - x3) * (x1 - x3);
        int d4 = (y1 - y3) * (y1 - y3);
        int d5 = (x2 - x3) * (x2 - x3);
        int d6 = (y2 - y3) * (y2 - y3);

        T a = std::sqrt(d1 + d2);
        T b = std::sqrt(d3 + d4);
        T c = std::sqrt(d5 + d6);

        T p = a + b + c;
        return {p, a, b, c};
    };

    {
        string line;
        auto fIn = openFileBufferedReader(STORAGE);
        while (fIn->readLine(line)) {
            const auto parts = parseIntegers(line);
            ASSERTEQ(parts.size(), 6);
            l.emplace(vector<Point>{make_pair(parts[0], parts[1]),
                                    make_pair(parts[2], parts[3]), make_pair(parts[4], parts[5])});
        }
        for (const auto& parts : l) {
            const auto [p, a, b, c] = points2p(parts[0].first, parts[0].second, parts[1].first, parts[1].second,
                                               parts[2].first, parts[2].second);
            s += p;
        }
    }

    tp::ThreadPool tp;

    std::mutex mtx;

    std::ofstream fFound;
    fFound.open(STORAGE, std::ios_base::app);

    auto f = [&](int x1) {
        LOG_EVERY_MS(INFO, 10000) << std::setprecision(10) << OUT(x1) << OUT(l.size()) << OUT(s);
        for (int x2 = -LIMIT; x2 < LIMIT; ++x2) {
            for (int y1 = -LIMIT; y1 < LIMIT; ++y1) {
                int k = x1 * x1 + y1 * y1 - x2 * x2;
                if (k < 0 || std::sqrt(k) != int(std::sqrt(k))) {
                    continue;
                }

                int y2 = std::sqrt(k);
                int x3 = 5 - x1 - x2;
                int y3 = -y1 - y2;

                auto [p, a, b, c] = points2p(x1, y1, x2, y2, x3, y3);
                if (p > 100000) {
                    break;
                }

                int o1 = x1 * x1 + y1 * y1;
                int o2 = x2 * x2 + y2 * y2;
                int o3 = x3 * x3 + y3 * y3;

                if (a + b > c && a + c > b && b + c > a && o1 == o2 && o1 == o3) {
                    std::vector<std::pair<int, int>> tmp = {{x1, y1}, {x2, y2}, {x3, y3}};
                    std::sort(std::begin(tmp), std::end(tmp));

                    {
                        std::unique_lock<std::mutex> lck(mtx);
                        bool found = l.count(tmp);

                        if (!found) {
                            fFound << tmp << endl;
                            l.emplace(tmp);
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
