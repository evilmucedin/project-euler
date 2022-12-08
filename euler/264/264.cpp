#include <glog/logging.h>
#include <lib/header.h>

#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

const int LIMIT = 3000;

int main() {
    int s = 0;
    std::vector<std::vector<std::pair<int, int>>> l;

    for (int x1 = -LIMIT; x1 < LIMIT; x1++) {
        LOG_EVERY_MS(INFO, 10000) << OUT(x1);
        for (int x2 = -LIMIT; x2 < LIMIT; x2++) {
            for (int y1 = -LIMIT; y1 < LIMIT; y1++) {
                int k = x1 * x1 + y1 * y1 - x2 * x2;
                if (k < 0 || std::sqrt(k) != int(std::sqrt(k))) {
                    continue;
                }

                int y2 = std::sqrt(k);
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
                if (p > 100000) {
                    break;
                }

                int o1 = x1 * x1 + y1 * y1;
                int o2 = x2 * x2 + y2 * y2;
                int o3 = x3 * x3 + y3 * y3;

                if (a + b > c && a + c > b && b + c > a && o1 == o2 && o1 == o3) {
                    std::vector<std::pair<int, int>> tmp = {{x1, y1}, {x2, y2}, {x3, y3}};
                    std::sort(std::begin(tmp), std::end(tmp));

                    bool found = false;
                    for (const auto& p : l) {
                        if (p == tmp) {
                            found = true;
                            break;
                        }
                    }

                    if (!found) {
                        l.push_back(tmp);
                        s += p;
                    }
                }
            }
        }
    }

    std::cout << s << std::endl;

    return 0;
}
