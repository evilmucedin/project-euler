#include "lib/primes.h"
#include "lib/threads.h"

u64 found1 = 0;
u64 found2 = 0;
u64 notfound1 = 0;
u64 notfound2 = 0;

using IType = i64;

static IType solve(IType n, IType m, IType totientN, IType totientM) {
    if ((totientM % n) == totientN) {
        ++found2;
        return totientM;
    }
    if ((totientN % m) == totientM) {
        ++found2;
        return totientN;
    }

    auto g1 = gcd(totientM, m);

    if ( 0 == (n % g1) && (totientN % g1) ) {
        ++notfound1;
        return 0;
    }

    auto g2 = gcd(totientN, n);

    if ( 0 == (m % g2) && (totientM % g2) ) {
        ++notfound1;
        return 0;
    }

    auto g3 = gcd(n, m);

    if ((totientN % g3) != (totientM % g3)) {
        ++notfound1;
        return 0;
    }

    // cerr << g1 << " " << g2 << endl;

    /*
    if (g2 % g1) {

                // auto res = totientM;
                // while ((res < n*m) && ((res % n) != totientN)) {
                //     res += m;
                // }
                assert(res >= n*m);

                ++notfound1;
                return 0;
            }
        }
    }
    */
    auto g = gcd(g1, g2);
    auto n1 = n / g;
    auto m1 = m / g;
    auto totientN1 = totientN / g;
    auto totientM1 = totientM / g;
    {
        IType a;
        IType b;
        egcd(n1, m1, &a, &b);
        IType res = totientM1*a*n1 + totientN1*b*m1;
        auto d = res/(m1*n1);
        res -= d*m1*n1;
        while (res < 0) {
            res += m1*n1;
        }
        while (res >= m1*n1) {
            res -= m1*n1;
        }
        if (((res % n1) == totientN1) && ((res % m1) == totientM1)) {
            ++found1;
            return res*g;
        }
    }

    // cerr << "?" << n1 << " " << m1 << " " << totientN1 << " " << totientM1 << " " << gcd(n1, m1) << endl;

    auto res = totientM1;
    while ((res < n1*m1) && ((res % n1) != totientN1)) {
        res += m1;
    }

    if (res < n1*m1) {
        ++found2;
        return res*g;
    } else {
        ++notfound2;
    }

    return 0;
}

static IType solve0(IType n, IType m, IType totientN, IType totientM) {
    auto res = totientM;
    while ((res < n*m) && ((res % n) != totientN)) {
        res += m;
    }
    if (res < n*m) {
        return res;
    }
    return 0;
}

int main() {
    cerr << solve(4, 6, 2, 4) << endl;
    cerr << solve(4, 6, 3, 4) << endl;
    cerr << solve(1000001, 1001920, 990000, 384000) << endl;
    cerr << solve0(1000001, 1001920, 990000, 384000) << endl;
    // return 0;
    static constexpr u32 kMax = 1005000;
    TotientErato totients(kMax + 10000);
    u128 result = 0;

    static constexpr int kNthreads = 2;
    auto solvePart = [&](int iThread) {
        u128 lresult = 0;
        for (IType n = 1000000; n < kMax; ++n) {
            if ( (n % kNthreads) != iThread ) {
                continue;
            }

            cerr << "..." << (u64)n << " " << found1 << " " << found2 << " " << notfound1 << " " << notfound2 << endl;
            const IType totientN = totients.totient_[n];
            for (IType m = n + 1; m < kMax; ++m) {
                const IType totientM = totients.totient_[m];
#ifndef NDEBUG
                auto res1 = solve(n, m, totientN, totientM);
                auto res2 = solve0(n, m, totientN, totientM);
                if (res1 != res2) {
                    cerr << "!" << n << " " << m << " " << totientN << " " << totientM << " " << res1 << " " << res2 << endl;
                }
#endif
                lresult += solve(n, m, totientN, totientM);
            }
        }
        result += lresult;
    };

    runInThreads(kNthreads, solvePart);

    cout << result << endl;
    return 0;
}
