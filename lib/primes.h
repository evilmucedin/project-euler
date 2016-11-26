#pragma once

#include <cassert>

#include "lib/header.h"
#include "lib/math.h"

struct Erato {
    Erato(u32 n);

    BoolVector sieve_;
    IntVector primes_;
};

struct PrimeFactor {
    u64 factor_;
    u32 power_;

    PrimeFactor();
    PrimeFactor(u64 factor, u32 power);
};

using PrimeFactors = vector<PrimeFactor>;

PrimeFactors factorization(u64 number, const Erato& erato);
u64 eulerTotient(u64 number, const Erato& erato);

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

