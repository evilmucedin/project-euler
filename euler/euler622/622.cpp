#include "lib/header.h"

#include "glog/logging.h"

IntVector riffleShuffle(const IntVector& v) {
    IntVector result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        if (i & 1) {
            result[i] = v[i / 2 + (v.size() / 2)];
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
        now = (2 * now) % (k - 1);
        ++it;
    }
    return it;
}

size_t find1(const IntVector& v) { return find(v.begin(), v.end(), 1) - v.begin(); }

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

    static constexpr size_t M = 60;

    i128 result = 0;
    /*
    for (size_t n = 2; n <= 10000000000ULL; n += 2) {
        if (f(n, M) == M) {
            cout << n << endl;
            result += n;
        }
        LOG_EVERY_MS(INFO, 1000) << OUT(n) << OUT(f(n, 2*M)) << OUT(result) << endl;
    }
    */

    static constexpr uint64_t N = (1ULL << M) - 1;
    for (uint64_t i = 1; i <= (1ULL << (M / 2)); i += 2) {
        if ((N % i) == 0) {
            auto ans1 = i + 1;
            auto ans2 = (N / i) + 1;
            if (f(ans1, M + 1) == M) {
                result += ans1;
            }
            if (f(ans2, M + 1) == M) {
                result += ans2;
            }
            // cout << OUT(i + 1) << OUT((N / i) + 1) << OUT(f(ans, M + 1)) << OUT(f(i + 1, M + 1)) << endl;
            // cout << OUT(i + 1) << OUT((N / i) + 1) << OUT(f(ans, M + 1)) << OUT(f(i + 1, M + 1)) << endl;
        }
    }

    cout << OUT(result) << endl;

    return 0;
}
