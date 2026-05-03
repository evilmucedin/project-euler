#include <cstdio>

#include <vector>

using namespace std;

typedef vector<int> TIntVector;
typedef vector<bool> TBoolVector;

typedef unsigned long long int ui64;
typedef long long int i64;

static const ui64 LIMIT = 100000000ULL*100000000;
// static const ui64 LIMIT = 1000000;

inline ui64 Power(ui64 n) {
    n -= 3;
    return n*(n + 1)*(n + 2)/6;
}

ui64 Calc(ui64 now, size_t count, size_t index, const TIntVector& primes) {
    if (now > LIMIT)
        return 0;

    i64 res = 0;
    if (count >= 4) {
        res = LIMIT / now;
        res *= Power(count);
        res *= ((count & 1) ? 1 : -1);
    }

    for (size_t i = index; i < primes.size(); ++i)
        res += Calc(now*primes[i], count + 1, i + 1, primes);

    return res;
}

int main() {
    TIntVector primes;
    {
        static const int N = 100;
        TBoolVector erato(N, true);

        for (size_t i = 2; i < N; ++i) {
            if (erato[i]) {
                primes.push_back(i);
                size_t j = i + i;
                while (j < N) {
                    erato[j] = false;
                    j += i;
                }
            }
        }
    }

    printf("%lld\n", -Calc(1, 0, 0, primes));

    return 0;
}
