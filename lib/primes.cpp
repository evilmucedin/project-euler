#include "primes.h"

TErato::TErato(size_t n)
    : sieve_(n, true)
{
    sieve_[0] = false;
    sieve_[1] = false;
}
