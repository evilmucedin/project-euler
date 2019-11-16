#include "lib/header.h"

#include "lib/primes.h"

#include "glog/logging.h"

static constexpr u64 N = 10000000000ULL;
// static constexpr u64 N = 1000000000ULL;

int main() {
    Erato erato(N + 10);

    u64 result = N;

    u64 top = sqrt(N);
    u64 i = 0;
    while ((i < erato.primes_.size()) && (erato.primes_[i] <= N)) {
        // LOG(INFO) << OUT(erato.primes_[i]);
        if (erato.primes_[i] <= top) {
            result -= erato.primes_[i];
        } else {
            result -= N / erato.primes_[i];
        }
        ++i;
    }

    LOG(INFO) << OUT(result);

    /*
    u64 result2 = 0;
    for (u64 i = 2; i <= N; ++i) {
        auto f = factorization(i, erato);
        if (f.back().factor_ < sqrt(i)) {
            ++result2;
        }
        // LOG_EVERY_MS(INFO, 1000) << i << " " << result;
    }

    LOG(INFO) << OUT(result2 + 1);
    */

    return 0;
}
