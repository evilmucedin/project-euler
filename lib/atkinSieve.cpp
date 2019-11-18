#include "atkinSieve.h"

#include <vector>

#include "glog/logging.h"

#include "header.h"

AtkinSieve::AtkinSieve(size_t n) : n_(n)
{
    BoolVector sieve(n, false);

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

    /*
    for (size_t i = 0; i <= n / 60; ++i) {
        LOG_EVERY_MS(INFO, 10000) << "Atkin sieve stage #1: " << 60 * i;
        for (auto x : SET) {
            size_t j = 60 * i + x;
            if (j < n) {
                sieve[j] = false;
            }
        }
    }
    */

    const size_t limit1 = sqrt(n / 4) + 1.4;
    for (size_t x = 1; x < limit1; ++x) {
        LOG_EVERY_MS(INFO, 10000) << "Atkin sieve stage #2: " << x;
        for (size_t y = 1; 4 * x * x + y * y < n; y += 2) {
            size_t z = 4 * x * x + y * y;
            if (z >= n) {
                break;
            }
            if (MASK1[z % 60]) {
                sieve[z] = !sieve[z];
            }
        }
    }

    const size_t limit2 = sqrt(n / 3) + 1.4;
    for (size_t x = 1; x < limit2; x += 2) {
        LOG_EVERY_MS(INFO, 10000) << "Atkin sieve stage #3: " << x;
        for (size_t y = 2; 3 * x * x + y * y < n; y += 2) {
            size_t z = 3 * x * x + y * y;
            if (z >= n) {
                break;
            }
            if (MASK2[z % 60]) {
                sieve[z] = !sieve[z];
            }
        }
    }

    const size_t limit3 = sqrt(n / 2) + 1.4;
    for (size_t x = 2; x < limit3; ++x) {
        LOG_EVERY_MS(INFO, 10000) << "Atkin sieve stage #4: " << x;
        for (ssize_t y = x - 1; y > 0; y -= 2) {
            size_t z = 3 * x * x - y * y;
            if (z < n) {
                if (MASK3[z % 60]) {
                    sieve[z] = !sieve[z];
                }
            }
        }
    }

    const size_t limit4 = sqrt(n / 60 / 60) + 1.4;
    for (size_t x = 0; x <= limit4 + 1; ++x) {
        LOG_EVERY_MS(INFO, 10000) << "Atkin sieve stage #5: " << x;
        for (auto y : SET) {
            size_t j = 60 * x + y;
            if (j * j < n && j >= 7) {
                if (sieve[j]) {
                    const size_t limit5 = n / j / j / 60 + 1;
                    for (size_t k = 0; k < limit5; ++k) {
                        for (auto c : SET) {
                            size_t z = j * j * (60 * k + c);
                            if (z < n) {
                                sieve[z] = false;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    primes_ = {2, 3, 5};
    for (size_t x = 0; x <= n / 60; ++x) {
        for (size_t y : SET) {
            size_t z = 60 * x + y;
            if (z >= n) {
                continue;
            }
            if (z < 7) {
                continue;
            }
            if (sieve[z]) {
                primes_.emplace_back(z);
            }
        }
    }
    cerr << "AtkinSieve is done." << endl;
}

size_t AtkinSieve::pi(size_t n) const {
    assert(n < n_);
    return upper_bound(primes_.begin(), primes_.end(), n) - primes_.begin();
}

size_t AtkinSieve::n() const {
    return n_;
}

