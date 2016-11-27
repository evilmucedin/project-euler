#include <cmath>

#include "lib/primes.h"

void solve(u32 n) {
    Erato erato(n);
    u64 result = 0;

    const auto& primes = erato.primes_;
    U64Set udivs;
    for (size_t i = 0; i < primes.size(); ++i) {
        if (0 == (i % 10000)) {
            cerr << "..." << i << endl;
        }
        u64 b = primes[i];
        auto divs = divisors(b + 1, erato);
        udivs.clear();
        for (size_t j = 0; j < divs.size(); ++j) {
            for (size_t k = j; k < divs.size(); ++k) {
                auto a = divs[j]*divs[k] - 1;
                if (udivs.count(a)) {
                    continue;
                }
                udivs.insert(a);
                if (a > 0 && a < b) {
                    if (erato.isPrime(a)) {
                        auto c = (b + 1)*(b + 1)/(a + 1) - 1;
                        if (c > b && c < n) {
                            if (erato.isPrime(c)) {
                                // cerr << a << " " << b << " " << c << endl;
                                result += a;
                                result += b;
                                result += c;
                            }
                        }
                    }
                }
            }
        }
    }

    cout << n << " " << result << endl;
}

int main() {
    solve(100);
    solve(100000000);

    return 0;
}
