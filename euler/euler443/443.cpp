#include "lib/primes.h"

int main() {
    static constexpr u64 kN = 1000000000000000ULL;
    const Erato erato(100000000);

    u64 n = 5;
    u64 g = 13;
    u64 steps = 0;
    while (true) {
        auto factors = factorization(g - n, erato);
        u64 div = kN;
        for (const auto& f: factors) {
            const auto mod = n % f.factor_;
            if (mod) {
                div = min(div, f.factor_ - mod);
            } else {
                div = 0;
                break;
            }
        }

        if (n + div + 1 >= kN) {
            break;
        }

        g += div + gcd(n + div, g + div);
        n += div + 1;
        ++steps;
        // cerr << n << " " << g << endl;
    }

    cout << g + (kN - n) + 1 << endl;
    cerr << steps << endl;

    return 0;
}
