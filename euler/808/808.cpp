#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/primes.h"
#include "lib/string.h"

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    Erato s(100000000);
    unordered_set<i64> rev;

    for (auto p : s.primes_) {
        i64 pp = sqr<i64>(p);
        auto s1 = to_string(pp);
        reverse(s1);
        i64 revPP = stringCast<i64>(s1);
        i64 sqRevPP = sqrt(revPP);
        for (int d = 0; d < 2; ++d) {
            if (sqRevPP * sqRevPP == revPP) {
                if (isPrime(sqRevPP, s)) {
                    if (revPP != pp) {
                        rev.emplace(pp);
                        rev.emplace(revPP);
                    }
                }
            }
            ++sqRevPP;
        }
        if (rev.size() >= 100) {
            LOG(INFO) << OUT(rev.size()) << OUT(sum(rev)) << OUT(p);
            break;
        }
    }

    vector<i64> revV(rev.begin(), rev.end());
    sort(revV);
    LOG(OK) << OUT(rev.size()) << OUT(sum(slice(revV, 0, 50)));

    return 0;
}
