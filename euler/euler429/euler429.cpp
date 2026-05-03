#include <cstdio>

#include <vector>

using namespace std;

typedef vector<int> TIntVector;
typedef vector<bool> TBoolVector;

static const size_t N = 100000000;
// static const size_t N = 4;

static const size_t MOD = 1000000000 + 9;

typedef unsigned long long int TLLI;

TLLI FastMod(TLLI base, TLLI power, TLLI mod)
{
    if (0 == power)
    {
        return 1;
    }

    if (power & 1)
    {
        return (base*FastMod(base, power - 1, mod)) % mod;
    }
    else
    {
        TLLI power2 = FastMod(base, power >> 1, mod);
        return (power2*power2) % mod;
    }
}

int main()
{
    TIntVector primes;
    {
        TBoolVector erato(N + 1, true);
        for (size_t i = 2; i <= N; ++i)
        {
            if (erato[i])
            {
                primes.push_back(i);
                for (size_t j = i + i; j < erato.size(); j += i)
                {
                    erato[j] = false;
                }
            }
        }
    }

    printf("Primes: %d\n", static_cast<int>(primes.size()));

    TIntVector power(primes.size());

    for (size_t i = 0; i < primes.size(); ++i)
    {
        if (!(i % 100000))
        {
            printf("...%d\n", static_cast<int>(i));
        }

        size_t ii = N;
        while (ii)
        {
            ii /= primes[i];
            power[i] += ii;
        }
    }
    printf("Factorization done.\n");

    TLLI result = 1;

    for (size_t i = 0; i < power.size(); ++i)
    {
        result = (result * (1 + FastMod(primes[i], 2*power[i], MOD))) % MOD;
    }

    printf("Result: %lld\n", result);

    return 0;
}
