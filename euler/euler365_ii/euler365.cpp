#include <cstdio>

#include <vector>

using namespace std;

typedef unsigned long long int TLLI;

typedef vector<int> TIntVector;
typedef vector<bool> TBoolVector;
typedef vector<TLLI> TLLIVector;

static const size_t N = 5000;
// static const size_t N = 4;

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

void FactorialFactorization(TLLI n, const TIntVector& primes, TLLIVector* result)
{
    result->resize(primes.size());
    for (size_t i = 0; i < primes.size(); ++i)
    {
        TLLI power = 0;
        TLLI nn = n;
        while (nn)
        {
            nn /= primes[i];
            power += nn;
        }
        (*result)[i] = power;
    }
}

TLLI BinomialModPrime(TLLI n, TLLI m, int prime)
{
    printf("%lld %lld %d\n", n, m, prime);
    if (m > n)
    {
        return 0;
    }
    if (n == 0 || m == 0)
    {
        return 0;
    }
    if (n == m || m == 1)
    {
        return 1;
    }
    if (n < 5)
    {
        return (BinomialModPrime(n - 1, m, prime) + BinomialModPrime(n - 1, m - 1, prime)) % prime;
    }
    if (n > prime)
    {
        TLLI subResult1 = BinomialModPrime(n / prime, m / prime, prime);
        TLLI subResult2 = BinomialModPrime(n % prime, m % prime, prime);
        return (subResult1*subResult2) % prime; 
    }
    else
    {
        int powerPrime = 0;
        for (TLLI i = m + 1; i <= n; ++i)
        {
            TLLI ii = i;
            while (0 == (i % prime))
            {
                ii /= prime;
                ++primePower;
            }
        }
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
                    primes.push_back(static_cast<int>(i));
                }
                for (size_t j = i + i; j < erato.size(); j += i)
                {
                    erato[j] = false;
                }
            }
        }
    }

    printf("Primes: %d\n", static_cast<int>(primes.size()));

    /*
    TLLIVector powerLB1;
    FactorialFactorization(LB, primes, &powerLB1);
    TLLIVector powerLB2;
    FactorialFactorization(UB - LB, primes, &powerLB2);
    TLLIVector powerUB;
    FactorialFactorization(UB, primes, &powerUB);

    TLLIVector primePowers(primes.size());
    size_t zeroes = 0;
    for (size_t i = 0; i < primes.size(); ++i)
    {
        TLLI primePower = powerUB[i] - powerLB1[i] - powerLB2[i];
        if (0 == primePower)
        {
            ++zeroes;
        }
        primePowers[i] = primePower;
        printf("%d %d\n", static_cast<int>(primes[i]), static_cast<int>(primePower));
    }
    printf("Zeroes: %d\n", static_cast<int>(zeroes));


    for (size_t i = 0; i < primes.size(); ++i)
    {
        if (!primePowers[i])
        {
            printf("...%d\n", static_cast<int>(i));
            int modi = BinomialModPrime(UB, LB, primes[i]);
            for (size_t j = 0; j < primes.size(); ++j)
            {
                if (!primePowers[j])
                {
                    int modj = BinomialModPrime(UB, LB, primes[j]);
                    for (size_t k = 0; k < primes.size(); ++k)
                    {
                        if (!primePowers[k])
                        {
                            int modk = BinomialModPrime(UB, LB, primes[k]);
                        }
                    }
                }
            }
        }
    }
    */

    for (size_t i = 0; i < primes.size(); ++i)
    {
        printf("...%d\n", static_cast<int>(i));
        int modi = BinomialModPrime(UB, LB, primes[i]);
        for (size_t j = 0; j < primes.size(); ++j)
        {
            int modj = BinomialModPrime(UB, LB, primes[j]);
            {
                for (size_t k = 0; k < primes.size(); ++k)
                {
                    int modk = BinomialModPrime(UB, LB, primes[k]);
                    printf("%d %d %d\n", modi, modj, modk);
                }
            }
        }
    }

    return 0;
}
