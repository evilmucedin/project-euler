#include <cstdio>

#include <vector>

using namespace std;

typedef vector<int> TIntVector;
typedef vector<bool> TBoolVector;

static const size_t N = 5000;
// static const size_t N = 4;

typedef unsigned long long int TLLI;

static const TLLI UB = 1000000000000000000ULL;
static const TLLI LB = 1000000000ULL;

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

void FactorialFactorization(TLLI n, const TIntVector& primes, TIntVector* result)
{
    result->resize(primes.size());
    for (size_t i = 0; i < primes.size(); ++i)
    {
        int power = 0;
        TLLI nn = n;
        while (nn)
        {
            nn /= primes[i];
            power += nn;
        }
        (*result)[i] = power;
    }
}

int main()
{
    TIntVector primes;
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

    TIntVector powerLB1;
    FactorialFactorization(LB, primes, &powerLB1);
    TIntVector powerLB2;
    FactorialFactorization(UB - LB, primes, &powerLB2);
    TIntVector powerUB;
    FactorialFactorization(UB, primes, &powerUB);

    for (size_t i = 0; i < primes.size(); ++i)
    {
        printf("%d %d\n", static_cast<int>(primes[i]), powerUB[i] - powerLB1[i] - powerLB2[i]);
    }

    return 0;
}
