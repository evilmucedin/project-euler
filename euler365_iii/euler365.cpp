#include <cstdio>

#include <cstdio>

#include <vector>

using namespace std;

typedef long long int i64;

typedef vector<int> TIntVector;
typedef vector<bool> TBoolVector;
typedef vector<i64> TI64Vector;

static const size_t N = 5000;
// static const size_t N = 4;

static const i64 UB = 1000000000000000000ULL;
static const i64 LB = 1000000000ULL;

i64 EGCD(i64 n, i64 m, i64* a, i64* b)
{
    // printf("!%lld %lld\n", n, m);
    if (0 == m)
    {
        *a = 1;
        *b = 0;
        return n;
    }
    i64 as;
    i64 bs;
    i64 s = EGCD(m, n % m, &as, &bs);
    // printf("!%lld %lld\n", as*m + bs*(n % m), s);
    *a = bs;
    *b = as - bs*(n / m);
    // printf("!!%lld %lld\n", *a*n + *b*m, s);
    return s;
}

i64 InvertModPrime(i64 x, i64 prime)
{
    i64 a;
    i64 b;
    i64 s = EGCD(x, prime, &a, &b);
    while (a < 0)
    {
        a += prime;
    }
    while (a > prime)
    {
        a -= prime;
    }
    if (1 != ((x*a) % prime))
    {
        throw std::exception();
    }
    // printf("%lld\n", (x*a) % prime);
    // printf("%lld %lld %lld %lld %lld\n", s, x, a, prime, (x*a) % prime);
    return a;
}

int BinomialCoefficientMod(i64 n, i64 m, int prime)
{
    if (m == 0 || m == n)
    {
        return 1;
    }
    if (m > n)
    {
        return 0;
    }
    if (n > prime)
    {
        i64 subTask1 = BinomialCoefficientMod(n / prime, m / prime, prime);
        i64 subTask2 = BinomialCoefficientMod(n % prime, m % prime, prime);
        return (subTask1*subTask2) % prime;
    }

    // printf("%lld %lld\n", n, m);

    int primePower = 0;
    i64 result = 1;
    for (i64 i = n - m + 1; i <= n; ++i)
    {
        i64 ii = i;
        while (0 == (ii % prime))
        {
            ++primePower;
            ii /= prime;
        }
        result = (result*ii) % prime;
    }
    for (i64 i = 1; i <= m; ++i)
    {
        i64 ii = i;
        while (0 == (ii % prime))
        {
            --primePower;
            ii /= prime;
        }
        result = (result*InvertModPrime(ii, prime)) % prime;
    }

    // printf("%lld %lld %d %lld\n", n, m, primePower, result);

    if (0 != primePower)
    {
        return 0;
    }
    else
    {
        return result;
    }
}

int main()
{
    TI64Vector primes;
    {
        TBoolVector erato(N + 1, true);
        for (size_t i = 2; i < N; ++i)
        {
            if (erato[i])
            {
                if (i > 1000)
                {
                    primes.push_back(i);
                }
                for (size_t j = i + i; j < erato.size(); j += i)
                {
                    erato[j] = false;
                }
            }
        }
    }

    printf("Primes: %d\n", static_cast<int>(primes.size()));

    TI64Vector remainder(primes.size());
    for (size_t i = 0; i < primes.size(); ++i)
    {
        remainder[i] = BinomialCoefficientMod(UB, LB, primes[i]);
        printf("%d %lld\n", i, remainder[i]);
    }

    i64 result = 0;
    for (size_t i = 0; i < primes.size(); ++i)
    {
        printf("%d\n", i);
        for (size_t j = i + 1; j < primes.size(); ++j)
        {
            for (size_t k = j + 1; k < primes.size(); ++k)
            {
                i64 m = primes[i]*primes[j]*primes[k];
                i64 ii = (m/primes[i]) % primes[i];
                i64 jj = (m/primes[j]) % primes[j];
                i64 kk = (m/primes[k]) % primes[k];
                // printf("%lld\n", (ii*InvertModPrime(ii, primes[i])) % primes[i]);
                // printf("%lld\n", (jj*InvertModPrime(jj, primes[j])) % primes[j]);
                // printf("%lld\n", (kk*InvertModPrime(kk, primes[k])) % primes[k]);
                i64 x = (remainder[i]*(m/primes[i])*InvertModPrime(m/primes[i], primes[i]) +
                        remainder[j]*(m/primes[j])*InvertModPrime(m/primes[j], primes[j]) +
                        remainder[k]*(m/primes[k])*InvertModPrime(m/primes[k], primes[k])) % m;
                // printf("%lld %lld %lld\n", (x % primes[i]) - remainder[i], (x % primes[j]) - remainder[j], (x % primes[k]) - remainder[k]);
                result += x;
            }
        }
    }
    printf("%lld\n", result);

    return 0;
}
