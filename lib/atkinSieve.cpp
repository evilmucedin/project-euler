#include "atkinSieve.h"

#include "glog/logging.h"

AtkinSieve::AtkinSieve(size_t n)
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
    cerr << "AtkinSieve is done." << endl;
}

bool AtkinSieve::isPrime(size_t n) const {
    assert(n < sieve_.size());
    return sieve_[n];
}

size_t AtkinSieve::pi(size_t n) const {
    assert(n < sieve_.size());
    return upper_bound(primes_.begin(), primes_.end(), n) - primes_.begin();
}

size_t AtkinSieve::n() const {
    return sieve_.size();
}

