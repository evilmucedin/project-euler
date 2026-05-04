#include "lib/primes.h"
#include "lib/math.h"

u64 powerSumModulo(u64 a, u64 n, u64 modulo) {
    if (0 == n) {
        return 1;
    }
    if (n & 1) {
        auto halfResult = powerSumModulo(a, n/2, modulo);
        return ((1 + a)*halfResult) % modulo;
    } else {
        return (1 + a*powerSumModulo(a, n - 1, modulo)) % modulo;
    }
}

void solve(u32 n) {
    TotientErato erato(n + 100);
    u64 result = 0;
    for (u32 i = 1; i <= n; i += 2) {
        if (1 == (i % 1000000)) {
            cerr << "..." << i << endl;
        }
        u64 iResult = erato.totient_[i];
        iResult %= i + 1;
        // iResult *= powerSumModulo(i, i - 1, i + 1);
        // iResult %= i + 1;
        // cerr << i << " " << iResult << endl;
        result += iResult;
    }
    cout << n << "\t" << result << endl;
}

int main() {
    solve(100);
    solve(5 * 100000000);
    return 0;
}
