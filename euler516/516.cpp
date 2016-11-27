#include "lib/primes.h"

static bool isHumming(int n) {
    while (0 == (n % 2)) {
        n /= 2;
    }
    while (0 == (n % 3)) {
        n /= 3;
    }
    while (0 == (n % 5)) {
        n /= 5;
    }
    return n == 1;
}

static void gen(u64 n, u64 limit, U64Set& set) {
    if (n > limit) {
        return;
    }
    if (set.count(n)) {
        return;
    }
    set.insert(n);
    gen(n*2, limit, set);
    gen(n*3, limit, set);
    gen(n*5, limit, set);
}

static void gen2(u128 n, u128 limit, const U64Vector& hummingPrimes, U64Set& set, u64& sum) {
    if (n > limit) {
        return;
    }
    if (set.count(n)) {
        return;
    }
    set.insert(n);
    if ((set.size() % 100000) == 0) {
        cerr << "..." << static_cast<u64>(n) << " " << set.size() << endl;
    }
    static constexpr u64 kMod = 1ULL << 32;
    sum = (sum + n) % kMod;
    gen2(n*2, limit, hummingPrimes, set, sum);
    gen2(n*3, limit, hummingPrimes, set, sum);
    gen2(n*5, limit, hummingPrimes, set, sum);
    for (auto prime : hummingPrimes) {
        if (n % prime) {
            gen2(n*prime, limit, hummingPrimes, set, sum);
        }
    }
}

static void solve(u64 n) {
    U64Set numbers;
    gen(1, n, numbers);
    cerr << "for " << n << " found " << numbers.size() << " Humming numbers." << endl;

    U64Vector hummingPrimes;
    for (auto n : numbers) {
        if (isPrime(n + 1)) {
            hummingPrimes.emplace_back(n + 1);
        }
    }
    cerr << "for " << n << " found " << hummingPrimes.size() << " Humming primes." << endl;

    u64 result = 0;
    U64Set set;
    gen2(1, n, hummingPrimes, set, result);

    cout << n << " " << result << endl;
}

int main() {
    solve(100);
    solve(1000000000000ULL);
    return 0;
}
