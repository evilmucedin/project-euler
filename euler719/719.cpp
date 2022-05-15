#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"

bool canMake(const IntVector& v, int start, i64 make) {
    if (start == v.size()) {
        return make == 0;
    }
    if (make < 0) {
        return false;
    }
    i64 next = 0;
    for (int i = start; i < v.size(); ++i) {
        next = 10 * next + v[i];
        if (next > make) {
            return false;
        }
        if (canMake(v, i + 1, make - next)) {
            return true;
        }
    }
    return false;
}

bool good(i64 n) {
    auto nn = n * n;
    auto digits = numToDigits(nn);
    return canMake(digits, 0, n);
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    i64 result = 0;

    for (i64 i = 2; i <= 1000000; ++i) {
        if (good(i)) {
            LOG(INFO) << OUT(i * i) << OUT(static_cast<double>(i) / 1000000);
            result += i * i;
        }
    }

    LOG(OK) << OUT(result);

    return 0;
}
