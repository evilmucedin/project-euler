#include "glog/logging.h"

#include "lib/header.h"
#include "lib/primes.h"
#include "lib/segmentTree.h"

static u64 solve(u64 u, u64 k) {
    auto size = u + 2*k + 10;

    IntVector cDiv(size);
    {
        LOG(INFO) << "factorization sieve";
        FactorizationErato erato(size);

        LOG(INFO) << "factorization";
        for (size_t i = 1; i < cDiv.size(); ++i) {
            u64 result = 1;
            for (const auto& factor: erato.factorize(i)) {
                result *= 1 + factor.power_;
            }
            cDiv[i] = result;
        }
    }

    LOG(INFO) << "segment tree";
    SegmentTree<int, Max<int>> maxTree(cDiv);

    LOG(INFO) << "solution";

    auto m = [&](int n, int k) {
        return maxTree.get(n, n + k);
    };

    auto s = [&](int u, int k) {
        u64 sum = 0;
        for (int n = 1; n <= u - k + 1; ++n) {
            sum += m(n, k);
        }
        return sum;
    };

    return s(u, k);
}

int main() {
    cout << solve(1000, 10) << endl;
    cout << solve(100000000, 100000) << endl;
    return 0;
}
