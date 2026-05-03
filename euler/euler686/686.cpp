#include "glog/logging.h"
#include "lib/header.h"

double log10(double x) { return log(x) / log(10.0); }

double frac(double x) {
    double intPart;
    return modf(x, &intPart);
}

int main() {
    int power = 0;
    int startWith123 = 0;

    static const double LOG2 = log10(2.0);

    static const double LOWER = frac(log10(123.0));
    static const double UPPER = frac(log10(124.0));

    while (startWith123 != 678910) {
        const double powerFrac = frac(LOG2 * power);
        if (powerFrac >= LOWER && powerFrac < UPPER) {
            ++startWith123;
        }
        ++power;
        LOG_EVERY_MS(INFO, 10000) << OUT(power) << OUT(startWith123);
    }

    /*
    IntVector p(1, 1);
    while (startWith123 != 678910) {
        for (auto& x: p) {
            x *= 2;
        }
        for (size_t i = 0; i + 1 < p.size(); ++i) {
            if (p[i] > 9) {
                p[i] -= 10;
                ++p[i + 1];
            }
        }
        if (p.back() > 9) {
            p.back() -= 10;
            p.emplace_back(1);
        }
        ++power;

        if (p.size() >= 3 && p[p.size() - 1] == 1 && p[p.size() - 2] == 2 && p[p.size() - 3] == 3) {
            ++startWith123;
            printf("%d %.10lf %.10lf %.10lf\n", power, power * LOG2, log10(123.0), log10(124.0));
            // LOG_EVERY_MS(INFO, 1000) << OUT(power) << OUT();
        }

        LOG_EVERY_MS(INFO, 10000) << OUT(power) << OUT(startWith123) << OUT(p.size());
    }
    */

    LOG(INFO) << OUT(power - 1);

    return 0;
}
