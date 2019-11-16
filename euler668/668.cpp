#include "lib/header.h"

#include "lib/primes.h"

#include "glog/logging.h"

static constexpr u64 N = 10000000000ULL;

int main() {
    Erato erato(sqrt(N) + 10);
    u64 result = 0;
    for (u64 i = 2; i <= N; ++i) {
        auto f = factorization(i, erato);
        if (f.back().factor_ < sqrt(i)) {
            ++result;
        }
        LOG_EVERY_MS(INFO, 1000) << i << " " << result;
    }

    LOG(INFO) << OUT(result + 1);

    return 0;
}
