#include "lib/primes.h"

static U64Vector fillFib(u64 m, u64 mod) {
    U64Vector result;
    result.reserve(m + 2);
    result.emplace_back(0);
    result.emplace_back(1);
    while (result.size() < m) {
        result.emplace_back( (result[result.size() - 1] + result[result.size() - 2]) % mod );
    }
    return result;
}

static U64Vector fill(u64 x, u64 n, u64 mod, const U64Vector& fib) {
    U64Vector result(n);
    u64 now = 1;
    for (u64 j = 0; j < n; ++j) {
        now = (now*x) % mod;
        result[j] = (result[j] + (now*fib[j]) % mod) % mod;
    }
    return result;
}

int main() {
    Erato e(1000);
    const auto facts = factorization(1307674368000, e);
    static constexpr u64 maxN = 100000;
    U64Vector rems;
    U64Vector bases;
    for (const auto& f : facts) {
        const u64 mod = power(f.factor_, static_cast<u64>(f.power_));
        const U64Vector fibs = fillFib(maxN, mod);
        cout << mod << " " << fibs << endl;
        for (int x = 1; x <= 100; ++x) {
            const U64Vector seq = fill(x, maxN, mod, fibs);
            cout << mod << " " << x << " " << seq << endl;
        }
    }
    u64 result;
    cout << crt(rems, bases, &result) << endl;
    cout << result << endl;
    return 0;
}
