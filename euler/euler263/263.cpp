#include <cassert>

#include "lib/primes.h"
#include "lib/console.h"

static constexpr int kLimit = 2000000000;
// static constexpr int kLimit = 1000000;
Erato erato(kLimit);

static bool isPractical(int n) {
    if (n > 4) {
        if ((n % 4) && (n % 6)) {
            return false;
        }
    }
    auto divs = divisors(n, erato);
    if (sum(divs) + 1 < 2*n) {
        return false;
    }
    // cerr << n << " " << divs << endl;
    I32Set numbers;
    numbers.insert(0);
    for (size_t i = 0; i + 1 < divs.size(); ++i) {
        auto div = divs[i];
        if (!numbers.count(div - 1)) {
            return false;
        }
        I32Set newNumbers;
        for (auto num: numbers) {
            newNumbers.insert(num);
            if (num + div <= n) {
                newNumbers.insert(num + div);
            }
        }
        numbers = std::move(newNumbers);
    }
    for (size_t i = 1; i < n; ++i) {
        if (!numbers.count(i)) {
            cerr << "Fail: " << n << " on " << i << endl;
            return false;
        }
    }
    return true;
}

static bool isPractical2(int n) {
    if (n < 1) {
        return false;
    }
    if (n == 1 || n == 2) {
        return true;
    }
    auto factor = factorization(n, erato);
    PrimeFactors subFactors;
    for (const auto& f : factor) {
        if (!subFactors.empty()) {
            if (f.factor_ > 1 + sum(divisors(subFactors))) {
                return false;
            }
        } else {
            if (f.factor_ != 2) {
                return false;
            }
        }
        subFactors.emplace_back(f);
    }
    return true;
}

static void solve() {
    u64 sum = 0;
    u32 index = 0;
    u32 deny1 = 0;
    u32 deny2 = 0;
    u32 deny3 = 0;
    u32 deny4 = 0;
    u32 deny5 = 0;
    for (size_t n = 10; (n + 9 < kLimit) && (4 != index); ++n) {
        if (erato.isPrime(n - 9) && erato.isPrime(n - 3) && erato.isPrime(n + 3) && erato.isPrime(n + 9)) {
            int cPrimes = 0;
            for (size_t i = n - 9; i <= n + 9; ++i) {
                if (erato.isPrime(i)) {
                    ++cPrimes;
                }
            }
            if (4 != cPrimes) {
                continue;
            }
            // cerr << "Prime: " << n << " " << deny1 << " " << deny2 << " " << deny3 << " " << deny4 << " " << deny5 << endl;
            if (!isPractical2(n - 4)) {
                ++deny1;
                continue;
            }
            if (!isPractical2(n + 4)) {
                ++deny2;
                continue;
            }
            if (!isPractical2(n - 8)) {
                ++deny3;
                continue;
            }
            if (!isPractical2(n)) {
                ++deny4;
                continue;
            }
            if (!isPractical2(n + 8)) {
                ++deny5;
                continue;
            }
            ++index;
            sum += n;
            cout << Modifier(FG_RED) << "Practical: " << n << " " << sum << " " << index << Modifier(FG_DEFAULT) << endl;
        }
    }
    cout << sum << endl;
}

static void checkPractical() {
    assert(isPractical(6));
    assert(isPractical2(6));
    assert(isPractical(429606));
    assert(isPractical2(429606));
    for (size_t i = 1; i < 500000; ++i) {
        if (isPractical(i) != isPractical2(i)) {
            cerr << "!" << i << endl;
        }
        if (isPractical(i) && (i < 50)) {
            cout << i << endl;
        }
    }
}

int main() {
    solve();
    // checkPractical();
    return 0;
}
