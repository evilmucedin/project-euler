#include "glog/logging.h"

#include "lib/header.h"
#include "lib/primes.h"

static u64 solve(u64 u, u64 k) {
    LOG(INFO) << "factorization";
    IntVector cDiv(u + k + 10);
    FactorizationErato erato(cDiv.size());
    for (size_t i = 1; i < cDiv.size(); ++i) {
        u64 result = 1;
        for (const auto& factor: erato.factorize(i)) {
            result *= 1 + factor.power_;
        }
        cDiv[i] = result;
    }
    return 0;
}

int main() {
    cout << solve(1000, 10) << endl;
    cout << solve(10000000, 100000) << endl;
    return 0;
}
