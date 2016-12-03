#include "lib/primes.h"

int main() {
    static constexpr u32 kMax = 1005000;
    TotientErato totients(kMax);
    u128 result = 0;
    u64 found1 = 0;
    u64 found2 = 0;
    u64 notfound1 = 0;
    u64 notfound2 = 0;
    for (i128 n = 1000000; n < kMax; ++n) {
        cerr << "..." << (u64)n << " " << found1 << " " << found2 << " " << notfound1 << " " << notfound2 << endl;
        const i128 totientN = totients.totient_[n];
        for (i128 m = n + 1; m < kMax; ++m) {
            const i128 totientM = totients.totient_[m];

            auto g1 = gcd(n, m);
            auto g2 = gcd(totientN, totientM);

            if (g2 % g1) {
                if ((totientM % n) == totientN) {
                    result += totientM;
                    ++found2;
                } else {
                    if ((totientN % m) == totientM) {
                        result += totientN;
                        ++found2;
                    } else {
                        ++notfound1;
                    }
                }

            } else {
                bool foundSolution = false;
                auto g = gcd(g1, g2);
                auto n1 = n / g;
                auto m1 = m / g;
                auto totientN1 = totientN / g;
                auto totientM1 = totientM / g;
                i128 a;
                i128 b;
                auto g2 = egcd(n1, m1, &a, &b);
                if (1 == g2) {
                    i128 res = totientM1*a*n1 + totientN1*b*m1;
                    auto d = res/(m1*n1);
                    res -= d*m1*n1;
                    while (res < 0) {
                        res += m1*n1;
                    }
                    if (((res % n1) == totientN1) && ((res % m1) == totientM1)) {
                        result += res*g;
                        foundSolution = true;
                        ++found1;
                    }
                }
                if (!foundSolution) {
                    auto res = totientM1;
                    while ((res < n1*m1) && ((res % n1) != totientN1)) {
                        res += m1;
                    }

                    if (res < n1*m1) {
                        result += res*g;
                        ++found2;
                    } else {
                        ++notfound2;
                    }
                }
            }
        }
    }
    cout << result << endl;
    return 0;
}
