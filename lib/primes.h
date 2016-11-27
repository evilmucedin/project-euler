#pragma once

#include <cassert>
#include <cmath>

#include "lib/header.h"
#include "lib/math.h"

struct Erato {
    Erato(u32 n);
    bool isPrime(u32 n) const;

    BoolVector sieve_;
    IntVector primes_;
};

struct PrimeFactor {
    u64 factor_;
    u32 power_;

    PrimeFactor();
    PrimeFactor(u64 factor, u32 power);
    bool operator<(const PrimeFactor& rhs) const;
};

using PrimeFactors = vector<PrimeFactor>;

PrimeFactors factorization(u64 number, const Erato& erato);
u64 eulerTotient(u64 number, const Erato& erato);
U64Vector divisors(const PrimeFactors& factorization);
U64Vector divisors(u64 number, const Erato& erato);
U64Vector divisors2(u64 number);

template<typename T>
bool isPrime(T number) {
    if (number < 2) {
        return false;
    }
    if (number < 4) {
        return true;
    }
    if (0 == (number & 1)) {
        return false;
    }
    T limit = static_cast<T>(::sqrt(static_cast<double>(number)) + 0.1);
    T div = 3;
    while (div <= limit && (number % div)) {
        div += 2;
    }
    return div > limit;
}

template<typename T>
T invertMod(T a, T b, const Erato& erato) {
    assert(1 == gcd(a, b));
    return powerMod(a, static_cast<T>(eulerTotient(b, erato)) - 1, b);
}

struct TotientErato {
    TotientErato(u32 n);

    BoolVector sieve_;
    IntVector totient_;
};

