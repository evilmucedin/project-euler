#include "primes.h"

#include <cmath>

#include "glog/logging.h"

Erato::Erato(size_t n)
    : sieve_(n, true)
{
    sieve_[0] = false;
    sieve_[1] = false;
    for (size_t i = 2; i < n; ++i) {
        if (sieve_[i]) {
            LOG_EVERY_MS(INFO, 10000) << "... erato " << i;
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

bool Erato::isPrime(size_t n) const {
    assert(n < sieve_.size());
    return sieve_[n];
}

size_t Erato::pi(size_t n) const {
    assert(n < sieve_.size());
    return upper_bound(primes_.begin(), primes_.end(), n) - primes_.begin();
}

size_t Erato::n() const {
    return sieve_.size();
}

PrimeFactor::PrimeFactor() = default;

PrimeFactor::PrimeFactor(u64 factor, u32 power)
    : factor_(factor)
    , power_(power)
{
}

bool PrimeFactor::operator<(const PrimeFactor& rhs) const {
    return factor_ < rhs.factor_;
}

ostream& operator<<(ostream& o, const PrimeFactor& v) {
    o << "{" << v.factor_ << "^" << v.power_ << "}";
    return o;
}

ostream& operator<<(ostream& o, const PrimeFactors& v) {
    bool first = true;
    for (const auto& f: v) {
        if (!first) {
            o << "*";
        }
        first = false;
        o << f;
    }
    return o;
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
    sort(result.begin(), result.end());
    return result;
}

u64 smallestDivisor(u64 number, const Erato& erato) {
    u64 top = ::sqrt(number) + 1;
    size_t index = 0;
    while (index < erato.primes_.size() && static_cast<u64>(erato.primes_[index]) <= top) {
        auto factor = erato.primes_[index];
        if (0 == (number % factor)) {
            return factor;
        }
        ++index;
    }
    return number;
}

bool isPrime(size_t number, const Erato& erato) {
    if (number < 2) {
        return false;
    }
    if (number < 4) {
        return true;
    }
    if (0 == (number & 1)) {
        return false;
    }
    int limit = static_cast<int>(::sqrt(static_cast<double>(number)) + 0.1);
    size_t iDiv = 1;
    const auto& primes = erato.primes_;
    while (iDiv < primes.size() && primes[iDiv] <= limit && (number % primes[iDiv])) {
        ++iDiv;
    }
    bool result = (iDiv >= primes.size()) || (0 != (number % primes[iDiv]));
    assert(number >= erato.sieve_.size() || result == erato.isPrime(number));
    return result;
}

static size_t phiPi(size_t m, size_t b, const Erato& erato) {
    if (0 == b) {
        return m;
    }
    if (m < 1) {
        return 0;
    }
    static constexpr size_t maxM = 100000;
    static constexpr size_t maxB = 100;
    static constexpr u64 kEmpty = -1;
    static const U64Vector dummy(maxB, kEmpty);
    static vector<U64Vector> cache(maxM, dummy);
    if (m < maxM && b < maxB) {
        auto& cacheItem = cache[m][b];
        if (kEmpty != cacheItem) {
            return cacheItem;
        }
        return cacheItem = phiPi(m, b - 1, erato) - phiPi(m/erato.primes_[b - 1], b - 1, erato);
    }
    return phiPi(m, b - 1, erato) - phiPi(m/erato.primes_[b - 1], b - 1, erato);
}

size_t pi(size_t number, const Erato& erato) {
    if (number < erato.n()) {
        return erato.pi(number);
    }

    size_t top = static_cast<size_t>(sqrt(number) + 0.0001);

    size_t y = pow(number, 1.0/3) + 1;

    auto p2 = [&](size_t m) {
        size_t result = 0;
        for (size_t p = y + 1; p <= top; ++p) {
            if (erato.isPrime(p)) {
                result += pi(m/p, erato) - pi(p, erato) + 1;
            }
        }
        return result;
    };

    size_t n = pi(y, erato);
    return phiPi(number, n, erato) + n - 1 - p2(number);
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

static void genDivisors(const PrimeFactors& factors, size_t index, u64 now, U64Vector* result) {
    if (index == factors.size()) {
        result->emplace_back(now);
    } else {
        u64 mul = 1;
        for (size_t i = 0; i <= factors[index].power_; ++i) {
            genDivisors(factors, index + 1, now*mul, result);
            mul *= factors[index].factor_;
        }
    }
}

U64Vector divisors(const PrimeFactors& factorization) {
    U64Vector result;
    genDivisors(factorization, 0, 1, &result);
    sort(result.begin(), result.end());
    return result;
}

U64Vector divisors(u64 number, const Erato& erato) {
    PrimeFactors primeFactors = factorization(number, erato);
    return divisors(primeFactors);
}

U64Vector divisors2(u64 number) {
    U64Vector result;
    u64 top = sqrt(static_cast<double>(number)) + 0.1;
    for (size_t i = 1; i <= top; ++i) {
        if (0 == (number % i)) {
            result.emplace_back(i);
            if (i*i != number) {
                result.emplace_back(number / i);
            }
        }
    }
    sort(result.begin(), result.end());
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

