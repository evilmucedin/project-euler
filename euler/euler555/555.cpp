#include "lib/header.h"
#include "lib/timer.h"
#include "lib/primes.h"

static u64 sum(u64 s, u64 k, u64 p) {
    return (2*p - 4*s + 2*k - (k - s - 1))*(k - s);
}

static u64 solve(u64 p, u64 m) {
    FactorizationErato erato(m + 1);
    Timer t("solve");
    u64 result = 0;
    for (u64 k = 2; k <= m; ++k) {
        for (auto d: divisors(erato.factorize(k))) {
            if (k != d) {
                result += sum(k - d, k, p);
            }
        }
    }
    return result/2;
}

int main() {
    cout << OUT(solve(10, 10)) << endl;
    cout << OUT(solve(1000, 1000)) << endl;
    cout << OUT(solve(1000000, 1000000)) << endl;
    return 0;
}
