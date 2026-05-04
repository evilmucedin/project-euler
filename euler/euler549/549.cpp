#include "lib/primes.h"

static void solve(u32 n) {
    Erato erato(n + 100);
    u64 result = 0;
    for (u32 i = 2; i <= n; ++i) {
        if (0 == (i % 1000000)) {
            cerr << "..." << i << endl;
        }

        auto factors = factorization(i, erato);
        u64 iResult = 1;
        for (auto& factor: factors) {
            u64 number = factor.factor_;
            i64 power = factor.power_;
            u64 mult = 0;
            while (power > 0) {
                mult += number;
                auto now = mult;
                while (now != 0 && 0 == (now % number)) {
                    now /= number;
                    --power;
                }
            }
            iResult = std::max(iResult, mult);
        }
        result += iResult;
    }
    cout << n << " " << result << endl;
}

int main() {
    solve(100);
    solve(100000000);

    return 0;
}
