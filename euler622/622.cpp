#include "lib/header.h"

#include "glog/logging.h"

IntVector riffleShuffle(const IntVector& v) {
    IntVector result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        if (i & 1) {
            result[i] = v[i/2 + (v.size()/2)];
        } else {
            result[i] = v[i / 2];
        }
    }
    return result;
}

int f(size_t k, size_t max) {
    size_t now = 2;
    int it = 1;
    while ((now != 1) && (it <= max)) {
        if (now < k/2) {
            now = 2*now;
        } else {
            now = 2*now - k + 1;
        }
        ++it;
    }
    return it;
}

size_t find1(const IntVector& v) {
    return find(v.begin(), v.end(), 1) - v.begin();
}

int main() {
    /*
    static constexpr size_t N = 86;
    IntVector v(N);
    for (size_t i = 0; i < N; ++i) {
        v[i] = i;
    }

    for (size_t i = 0; i < 9; ++i) {
        cout << OUT(v) << OUT(find1(v)) << endl;
        v = riffleShuffle(v);
    }
    */

    int64_t result = 0;
    for (size_t n = 2; n <= 10000000000ULL; n += 2) {
        if (f(n, 60) == 60) {
            cout << n << endl;
            result += n;
        }
        LOG_EVERY_MS(INFO, 1000) << OUT(n) << OUT(f(n, 100)) << endl;
    }
    cout << OUT(result) << endl;

    return 0;
}
