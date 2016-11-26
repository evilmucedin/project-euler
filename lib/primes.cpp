#include "primes.h"

#include <cmath>

Erato::Erato(u32 n)
    : sieve_(n, true)
{
    sieve_[0] = false;
    sieve_[1] = false;
    for (size_t i = 2; i < n; ++i) {
        if (sieve_[i]) {
            primes_.emplace_back(i);
            size_t j = i + i;
            while (j < n) {
                sieve_[j] = false;
                j += i;
            }
        }
    }
    cerr << "Erato is done." << endl;
}

bool Erato::isPrime(u32 n) const {
    assert(n < sieve_.size());
    return sieve_[n];
}

PrimeFactor::PrimeFactor() = default;

PrimeFactor::PrimeFactor(u64 factor, u32 power)
    : factor_(factor)
    , power_(power)
{
}

PrimeFactors factorization(u64 number, const Erato& erato) {
    u64 top = ::sqrt(number) + 1;
    PrimeFactors result;
    u64 now = number;
    size_t index = 0;
    while (index < erato.primes_.size() && static_cast<u64>(erato.primes_[index]) <= top) {
        auto factor = erato.primes_[index];
        if (0 == (now % factor)) {
            i32 power = 0;
            while (0 == (now % factor)) {
                now /= factor;
                ++power;
            }
            result.emplace_back(factor, power);
            top = ::sqrt(now) + 1;
        }
        ++index;
    }
    if (1 != now) {
        result.emplace_back(now, 1);
    }
    return result;
}

u64 eulerTotient(u64 number, const Erato& erato) {
    auto factors = factorization(number, erato);
    u64 result = 1;
    for (const auto& factor : factors) {
        result *= factor.factor_ - 1;
        result *= power<u64>(factor.factor_, factor.power_ - 1);
    }
    return result;
}

TotientErato::TotientErato(u32 n)
    : sieve_(n, true)
    , totient_(n)
{
    for (size_t i = 0; i < totient_.size(); ++i) {
        totient_[i] = i;
    }
    sieve_[0] = false;
    sieve_[1] = false;
    for (size_t i = 2; i < n; ++i) {
        if (sieve_[i]) {
            size_t j = i;
            while (j < n) {
                sieve_[j] = false;
                totient_[j] = (totient_[j]/i)*(i - 1);
                j += i;
            }
        }
    }
    cerr << "TotientErato is done." << endl;
}

