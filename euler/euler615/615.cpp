#include <queue>

#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/primes.h"

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    Erato erato(100000000);

    static constexpr size_t K = 1000000;
    static constexpr size_t NUM_MUL = 1000000;
    static constexpr size_t NUM_NOT_2 = 30;

    // static constexpr size_t K = 5;
    // static constexpr size_t NUM_MUL = 5;
    // static constexpr size_t NUM_NOT_2 = 5;

    static constexpr size_t NUM2 = NUM_MUL - NUM_NOT_2;
    static constexpr size_t MOD = 123454321;
    static constexpr size_t MAX_BITS = 50;

    priority_queue<i128, I128Vector, greater<i128>> q;
    q.emplace(1ULL << NUM_NOT_2);

    REP(it, K - 1) {
        LOG_EVERY_MS(INFO, 1000) << OUT(it) << OUT(q.size()) << OUT(q.top());
        LOG(INFO) << OUT(it) <<  OUT(q.top()) << OUT(q.size());

        const auto cur = q.top();
        q.pop();

        while (!q.empty() && (q.top() == cur)) {
            q.pop();
        }

        for (auto p : erato.primes_) {
            i128 nw = cur * static_cast<i128>(p);
            if (nw < (static_cast<i128>(1) << MAX_BITS)) {
                if (nw > cur) {
                    q.push(nw);
                }
            } else {
                break;
            }
        }

        for (auto p : erato.primes_) {
            if (p == 2) {
                continue;
            }

            i128 nw = static_cast<i128>(p) * static_cast<i128>(cur / 2);
            if (nw < (static_cast<i128>(1) << MAX_BITS)) {
                if (nw > cur) {
                    q.push(nw);
                }
            } else {
                break;
            }
        }
    }

    i128 res = q.top();
    REP(_, NUM2) { res = (res * 2) % MOD; }

    LOG(INFO) << OUT(res);

    return 0;
}
