#include "lib/primes.h"
#include "lib/math.h"

void solve(int n) {
    Erato erato(n + 100);
    u64 result = 0;
    for (u32 i = 1; i <= n; ++i) {
        if (0 == (i % 1000000)) {
            cerr << "..." << i << endl;
        }
        u64 iResult = eulerTotient(i, erato) % (i + 1);
        iResult *= (powerMod(i, i, i + 1) + i) % (i + 1);
        iResult %= i + 1;
        result += iResult;
    }
    cout << n << "\t" << result << endl;
}

int main() {
    solve(100);
    return 0;
}
