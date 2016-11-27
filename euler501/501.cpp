#include "lib/primes.h"

void solve(u64 n) {
    Erato erato(n + 1000);

    u64 result = 0;
    const auto& primes = erato.primes_;
    for (size_t i = 0; i < primes.size(); ++i) {
        u64 p1 = primes[i];

        auto toP2 = upper_bound(primes.begin(), primes.end(), pow(static_cast<long double>(n/p1), 1.0/3));
        if (toP2 > primes.begin() + i) {
            result += toP2 - primes.begin() - i - 1;
        }

        toP2 = upper_bound(primes.begin(), primes.end(), n/p1/p1/p1);
        if (toP2 > primes.begin() + i) {
            result += toP2 - primes.begin() - i - 1;
        }

        for (size_t j = i + 1; j < primes.size(); ++j) {
            u64 p2 = primes[j];
            u64 p3Limit = n/p1/p2;
            if (p3Limit > p2) {
                auto toP3 = upper_bound(primes.begin(), primes.end(), p3Limit);
                if (toP3 > primes.begin() + j) {
                    result += toP3 - primes.begin() - j - 1;
                }
            }
        }
    }

    u64 limit8 = pow(static_cast<long double>(n), 1./7);
    result += upper_bound(primes.begin(), primes.end(), limit8) - primes.begin();

    cout << n << " " << result << endl;
}

int main() {
    solve(100);
    solve(1000);
    solve(1000000);

    return 0;
}
