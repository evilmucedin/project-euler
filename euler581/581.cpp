#include <set>

#include <glog/logging.h>

#include "lib/header.h"
#include "lib/primes.h"

int main() {
    Erato e(48);
    LOG(INFO) << OUT(e.primes_);
    set<uint64_t> q;
    q.insert(1);
    uint64_t prev = 0;
    uint64_t sum = 0;
    while (q.size() < 400000000) {
        auto now = *(q.begin());
        q.erase(q.begin());
        if (now == prev + 1) {
            sum += prev;
            cout << prev << " " << now << " " << sum << endl;
        }
        LOG_EVERY_MS(INFO, 10000) << q.size() << " " << *(q.begin());
        prev = now;
        for (int p : e.primes_) {
            q.insert(p*now);
        }
    }
    return 0;
}
