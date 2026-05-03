#include <cstdio>

#include <vector>
#include <map>

using namespace std;

typedef vector<int> TIntVector;
typedef vector<bool> TBoolVector;

typedef unsigned long long int ui64;
typedef long long int i64;
typedef map<ui64, i64> THash;

template<typename T>
T GCD(T a, T b) {
    while (b) {
        T temp = a;
        a = b;
        b = temp % b;
    }
    return a;
}

template<typename T>
T LCM(T a, T b) {
    return (a/GCD(a, b))*b;
}

static const bool VERBOSE = true;

static const ui64 LIMIT = 100000;

THash stat;

void Factor(ui64 number, const TIntVector& numbers, size_t* len) {
    bool first = true;
    *len = 0;
    for (size_t i = 0; (i < numbers.size()) && (1 != number); ++i) {
        while (0 == (number % numbers[i])) {
            if (VERBOSE) {
                if (!first)
                    printf(" ");
                printf("%d", numbers[i]);
            }
            number /= numbers[i];
            first = false;
            ++(*len);
        }
    }
    if (VERBOSE) {
        if (1 != number)
            printf("!%lld", number);
    }
}

ui64 Calc(ui64 now, size_t count, size_t index, const TIntVector& numbers, const TIntVector& primes) {
    if (now > LIMIT)
        return 0;

    const ui64 res0 = LIMIT / now;
    ui64 res = res0;
    for (size_t i = index; i < numbers.size(); ++i) {
        const ui64 nw = LCM(now, (ui64)numbers[i]);
        res -= Calc(nw, count + 1, i + 1, numbers, primes);
    }
    stat[now] += ((count & 1) ? 1 : -1)*res0;

    if (VERBOSE) {
        printf("%lld %d %d %lld %lld [", now, count, index, res, res0);
        size_t dummy;
        Factor(now, primes, &dummy);
        printf("]\n");
    }

    return res;
}

ui64 Power(ui64 a, size_t p) {
    ui64 result = 1;
    for (size_t i = 0; i < p; ++i)
        result *= a;
    return result;
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

    TIntVector numbers4;
    for (size_t i1 = 0; i1 < primes.size(); ++i1)
        for (size_t i2 = i1 + 1; i2 < primes.size(); ++i2)
            for (size_t i3 = i2 + 1; i3 < primes.size(); ++i3)
                for (size_t i4 = i3 + 1; i4 < primes.size(); ++i4)
                    numbers4.push_back( primes[i1]*primes[i2]*primes[i3]*primes[i4] );

    ui64 stupido = 0;
    for (size_t i = 1; i < LIMIT; ++i) {
        size_t count = 0;
        for (size_t j = 0; j < primes.size(); ++j)
            if (!(i % primes[j]))
                ++count;
        if (count >= 4)
            ++stupido;
    }

    ui64 res = LIMIT - Calc(1, 0, 0, numbers4, primes);

    i64 res2 = 0;
    i64 res3 = 0;
    for (THash::const_iterator toHash = stat.begin(); toHash != stat.end(); ++toHash) {
        if (VERBOSE)
            printf("! %lld %lld [", toHash->first, toHash->second);
        res2 += toHash->second;
        size_t len;
        Factor(toHash->first, primes, &len);
        i64 guess = (LIMIT / toHash->first) * ((len & 1) ? -1 : 1) * Power(4, len - 4);
        res3 += guess;
        if (VERBOSE)
            printf("] %lld\n", guess);
    }
    printf("%lld %lld %lld %lld\n", res, stupido, (i64)LIMIT + res2, res3);

    return 0;
}
