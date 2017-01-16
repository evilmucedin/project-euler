#include "glog/logging.h"

#include "lib/primes.h"
#include "lib/math.h"

u64 solve(u64 n) {
    Erato erato(sqrt(n) + 1000);

    u64 result1 = 0;
    u64 result2 = 0;
    u64 result3 = 0;
    u64 result4 = 0;
    const auto& primes = erato.primes_;
    for (size_t i = 0; i < primes.size(); ++i) {
        LOG_EVERY_MS(INFO, 1000) << i;
        u64 p1 = primes[i];

        auto pi2 = pi(pow(static_cast<long double>(n/p1), 1.0/3), erato);
        if (pi2 > i) {
            result1 += pi2 - i - 1;
        } else {
            break;
        }
    }

    for (size_t i = 0; i < primes.size(); ++i) {
        LOG_EVERY_MS(INFO, 1000) << i;

        u64 p1 = primes[i];

        auto pi2 = pi(n/p1/p1/p1, erato);
        if (pi2 > i) {
            result2 += pi2 - i - 1;
        } else {
            break;
        }
    }

    for (size_t i = 0; i < primes.size(); ++i) {
        LOG_EVERY_MS(INFO, 1000) << i;

        u64 p1 = primes[i];
        if (n < p1*p1*p1) {
            break;
        }

        for (size_t j = i + 1; j < primes.size(); ++j) {
            LOG_EVERY_MS(INFO, 1000) << OUT(i) << OUT(j);

            u64 p2 = primes[j];
            u64 p3Limit = n/p1/p2;
            if (p3Limit > p2) {
                auto pi3 = pi(p3Limit, erato);
                if (pi3 > j) {
                    result3 += pi3 - j - 1;
                } else {
                    break;
                }
            } else {
                break;
            }
        }
    }

    u64 limit8 = pow(static_cast<long double>(n), 1./7);
    result4 = pi(limit8, erato);

    auto result = result1 + result2 + result3 + result4;
    cerr << OUT(result1) << OUT(result2) << OUT(result3) << OUT(result4) << OUT(result) << endl;
    cout << n << " " << result << endl;
    return result;
}

int main() {
    for (size_t i = 1; i <= 8; ++i) {
        cout << i << " " << solve(power<u64>(10, i)) << endl;
    }
    cout << 12 << " " << solve(power<u64>(10, 12)) << endl;

    return 0;
}
