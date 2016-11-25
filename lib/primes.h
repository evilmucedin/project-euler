#pragma once

#include "lib/header.h"

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
