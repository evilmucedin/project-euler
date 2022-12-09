#include <glog/logging.h>
#include <tqdm/tqdm.h>

#include <lib/header.h>
#include <lib/thread-pool/threadPool.h>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>

const int LIMIT = 400;

vector<pair<int, int>> produceY1Y2(int a) {
    a = abs(a);
    int top = sqrt(a);
    vector<pair<int, int>> result;
    for (int i = 1; i <= top; ++i) {
        if ((a % i) == 0) {
            int d1 = i;
            int d2 = a / i;
            int k = (d1 + d2) / 2;
            int y1 = d2 - k;
            result.emplace_back(make_pair(y1, k));

            d1 = a / i;
            d2 = i;
            k = (d1 + d2) / 2;
            y1 = d2 - k;
            result.emplace_back(make_pair(y1, k));
        }
    }
    return result;
}

int main() {
    tp::ThreadPool tp;

    std::mutex mtx;
    long double s = 0;
    std::unordered_set<std::vector<std::pair<int, int>>> l;

    auto f = [&](int x1) {
        LOG_EVERY_MS(INFO, 10000) << OUT(x1) << OUT(l.size()) << OUT(s);
        for (int x2 = -LIMIT; x2 < LIMIT; ++x2) {
            for (auto [y1, y2] : produceY1Y2(x1 * x1 - x2 * x2)) {
                int x3 = 5 - x1 - x2;
                int y3 = -y1 - y2;

                int d1 = (x1 - x2) * (x1 - x2);
                int d2 = (y1 - y2) * (y1 - y2);
                int d3 = (x1 - x3) * (x1 - x3);
                int d4 = (y1 - y3) * (y1 - y3);
                int d5 = (x2 - x3) * (x2 - x3);
                int d6 = (y2 - y3) * (y2 - y3);

                double a = std::sqrt(d1 + d2);
                double b = std::sqrt(d3 + d4);
                double c = std::sqrt(d5 + d6);

                double p = a + b + c;
                if (p > 50) {
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
                            l.emplace(tmp);
                            s += p;
                        }
                    }
                }
            }
        }
    };

    tp.runRange(f, -LIMIT, LIMIT);

    std::cout << std::setprecision(10) << s << std::endl;

    return 0;
}
