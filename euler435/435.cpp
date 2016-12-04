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

using FibPair = pair<u64, u64>;
using FibPairs = vector<FibPair>;

static FibPairs fill(u64 x, u64 n, u64 mod, const U64Vector& fib) {
    U64Vector result(n);
    result[0] = 0;
    u64 now = x;
    for (u64 j = 1; j < n; ++j) {
        // cerr << j << " " << fib[j] << " " << now << endl;
        result[j] = (result[j - 1] + ((now*fib[j]) % mod)) % mod;
        now = (now*x) % mod;
    }
    FibPairs fibResult;
    for (u64 j = 1; j < n; ++j) {
        fibResult.emplace_back(result[j - 1], result[j]);
    }
    return fibResult;
}

int main() {
    /*
    const auto tfibs = fillFib(10, 1000000);
    cerr << tfibs << endl;
    const auto tseq = fill(11, 10, 1000000, tfibs);
    cerr << tseq << endl;
    return 0;
    */

    Erato e(1000);
    static constexpr u128 kMod = 1307674368000;
    static constexpr u128 kIndex = 1000000000000000ULL;
    const auto facts = factorization(kMod, e);
    static constexpr u128 maxN = 100000;
    I128Vector rems;
    I128Vector bases;
    for (const auto& f : facts) {
        const u128 mod = power(static_cast<u128>(f.factor_), static_cast<u128>(f.power_));
        const auto fibs = fillFib(maxN, mod);
        u64 result = 0;
        for (int x = 1; x <= 100; ++x) {
            const auto seq = fill(x, maxN, mod, fibs);
            Cycle c;
            if (detectCycle(seq, &c)) {
                result = (result + getFromCycle(seq, c, kIndex).first) % mod;
            } else {
                cerr << "Cycle not detected" << endl;
            }
        }
        rems.emplace_back(result);
        bases.emplace_back(mod);
    }
    i128 result;
    cout << crt(rems, bases, &result) << endl;
    cout << result << endl;
    return 0;
}
