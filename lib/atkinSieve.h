#pragma once

#include "lib/header.h"

struct AtkinSieve {
    AtkinSieve(size_t n);
    bool isPrime(size_t n) const;
    size_t pi(size_t n) const;
    size_t n() const;

    size_t n_;
    SizeTVector primes_;
};
