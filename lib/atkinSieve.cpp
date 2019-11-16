#include "atkinSieve.h"

#include <vector>

#include "glog/logging.h"

#include "header.h"

AtkinSieve::AtkinSieve(size_t n)
    : sieve_(n, true)
{
    sieve_[0] = false;
    sieve_[1] = false;
    sieve_[2] = true;
    sieve_[3] = true;
    sieve_[5] = true;

    auto bitmask = [](const vector<size_t>& v, size_t n) {
        vector<bool> mask(n);
        for (auto x: v) {
            mask[x] = true;
        }
        return mask;
    };

    static const vector<size_t> SET1{1, 13, 17, 29, 37, 41, 49, 53};
    static const vector<bool> MASK1 = bitmask(SET1, 60);
    static const vector<size_t> SET2{7, 19, 31, 43};
    static const vector<bool> MASK2 = bitmask(SET2, 60);
    static const vector<size_t> SET3{11, 23, 47, 59};
    static const vector<bool> MASK3 = bitmask(SET3, 60);
    static const vector<size_t> SET = sorted(cat(SET1, cat(SET2, SET3)));

    for (size_t i = 0; i <= n / 60; ++i) {
        for (auto x : SET) {
            size_t j = 60 * i + x;
            if (j < n) {
                sieve_[j] = false;
            }
        }
    }

    for (size_t x = 1; 4 * x * x < n; ++x) {
        for (size_t y = 1; 4 * x * x + y * y < n; y += 2) {
            size_t z = 4 * x * x + y * y;
            if (MASK1[z % 60]) {
                sieve_[z] = !sieve_[z];
            }
        }
    }

    for (size_t x = 1; 3 * x * x < n; x += 2) {
        for (size_t y = 2; 3 * x * x + y * y < n; y += 2) {
            size_t z = 3 * x * x + y * y;
            if (MASK2[z % 60]) {
                sieve_[z] = !sieve_[z];
            }
        }
    }

    for (size_t x = 2; 3 * x * x - (x - 1) * (x - 1) < n; ++x) {
        for (size_t y = x - 1; (3 * x * x > y * y) && (y > 0); y -= 2) {
            size_t z = 3 * x * x - y * y;
            if (z < n) {
                if (MASK3[z % 60]) {
                    sieve_[z] = !sieve_[z];
                }
            }
        }
    }

    for (size_t x = 0; x <= sqrt(n / 60 / 60); ++x) {
        for (auto y : SET) {
            size_t j = 60 * x + y;
            if (j * j < n && j >= 7) {
                if (sieve_[j]) {
                    for (size_t k = 0; k < n / j / j / 60; ++k) {
                        for (auto x : SET) {
                            size_t z = j * j * (60 * k + x);
                            if (z < n) {
                                sieve_[z] = false;
                            }
                        }
                    }
                }
            }
        }
    }

    for (size_t i = 2; i < n; ++i) {
        if (sieve_[i]) {
            primes_.emplace_back(i);
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

