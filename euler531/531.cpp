#include "lib/primes.h"

int main() {
    static constexpr u32 kMax = 1005000;
    TotientErato totients(kMax);
    u64 result = 0;
    u64 found = 0;
    u64 notfound = 0;
    for (u128 n = 1000000; n < kMax; ++n) {
        cerr << "..." << (u64)n << " " << found << " " << notfound << endl;
        u128 totientN = totients.totient_[n];
        for (u128 m = n + 1; m < kMax; ++m) {
            u128 totientM = totients.totient_[m];

            u128 a;
            u128 b;
            auto g = egcd(n, m, &a, &b);
            if (1 == g) {
                u128 res = totientM*a*(u128)n + totientN*b*(u128)m;
                auto d = res/(m*n);
                res -= d*m*n;
                while (res < 0) {
                    res += m*n;
                }
                if (res % n != totientN || res % m != totientM) {
                    cout << "!" << (u64)res << " " << (u64)(res % n) << " " << (u64)totientN << endl;
                }
            } else {
                auto res = totientM;
                while ( (res < n*m) && ((res % n) != totientN) ) {
                    res += m;
                }

                if (res < n*m) {
                    result += res;
                    ++found;
                } else {
                    ++notfound;
                }
            }
        }
    }
    cout << result << endl;
    return 0;
}
