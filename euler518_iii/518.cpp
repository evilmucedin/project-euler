#include <cmath>

#include "lib/primes.h"

void solve(u32 n) {
    Erato erato(n);
    u64 result = 0;
    const auto& primes = erato.primes_;
    for (size_t i = 0; i < primes.size(); ++i) {
        u64 b = primes[i] + 1;
        size_t l = 0;
        size_t rr = primes.size() - 1;
        size_t rl = i + 1;
        size_t value = (b*b)/(primes[l] + 1) - 1;
        while (rl + 1 < rr) {
            size_t mid = (rl + rr) / 2;
            if (primes[mid] == value) {
                rr = mid;
                break;
            } else if (primes[mid] < value) {
                rl = mid;
            } else {
                rr = mid;
            }
        }
        size_t r = rr;
        if (r < i + 1) {
            r = i + 1;
        }
        while (r < primes.size() && (primes[l] + 1)*(primes[r] + 1) <= b*b) {
            ++r;
        }
        r = std::min(primes.size() - 1, r);
        while (l < i && i < r) {
            u64 a = primes[l] + 1;
            u64 c = primes[r] + 1;
            if (a*c == b*b) {
                result += a;
                result += b;
                result += c;
                result -= 3;
                // cerr << a - 1 << " " << b - 1 << " " << c - 1 << endl;
                ++l;
                --r;
            } else if (a*c < b*b) {
                ++l;
            } else {
                --r;
            }
        }
        if (0 == (i % 1000)) {
            cerr << "..." << l << " " << i << " " << r << " " << rr << " " << result << endl;
        }
    }
    cout << n << " " << result << endl;
}

int main() {
    solve(100);
    solve(100000000);

    return 0;
}
