#include "glog/logging.h"

#include "lib/header.h"
#include "lib/primes.h"

using namespace google;

static constexpr i64 kLimit = 100000000000LL;
static constexpr auto kFactor = 2017;
Erato erato(std::min(i64(1LL << 24), kLimit));
using InclusionExclusion = map<i64, int>;

struct SigmaPrime {
    SigmaPrime() {
        for (auto p: erato.primes_) {
            i128 power = 1;
            int iPower = 1;
            while (power <= kLimit) {
                i128 sigmaFactor = (power*p - 1)/(p - 1);
                if ((sigmaFactor % kFactor) == 0) {
                    divisors_[power] = 1;
                    if (power*p <= kLimit) {
                        divisors_[power*p] = -1;
                    }
                    cerr << power << " " << divisors_.size() << " " << p << " " << iPower << " " << (p % kFactor) << endl;
                }
                power *= p;
                ++iPower;
            }
        }

        i64 p = kFactor - 1;
        while (p <= kLimit) {
            if (isPrime(p, erato)) {
                LOG_EVERY_MS(INFO, 1000) << static_cast<double>(p)/kLimit;
                divisors_[p] = 1;
            }
            p += kFactor;
        }

        cerr << "SigmaPrime is done." << endl;
    }

    InclusionExclusion divisors_;
};

static const SigmaPrime sigmaPrime;

void solve(i128 limit) {
    InclusionExclusion inclusionExclusion;
    inclusionExclusion.emplace(1, -1);

    bool modify = true;
    while (modify) {
        modify = false;
        InclusionExclusion nextInclusionExclusion = inclusionExclusion;
        for (auto pair: inclusionExclusion) {
            for (auto d: sigmaPrime.divisors_) {
                if (1 != gcd(pair.first, d.first)) {
                    continue;
                }
                auto num = static_cast<i128>(pair.first)*static_cast<i128>(d.first);
                auto sign = -pair.second*d.second;
                if (num <= limit) {
                    if (nextInclusionExclusion.count(num) == 0) {
                        nextInclusionExclusion.emplace(num, sign);
                        modify = true;
                    } else {
                        CHECK_EQ(nextInclusionExclusion[num], sign);
                    }
                } else {
                    break;
                }
            }
        }
        inclusionExclusion = std::move(nextInclusionExclusion);
    }
    cerr << sigmaPrime.divisors_.size() << " " << inclusionExclusion.size() << endl;
    // cerr << inclusionExclusion << endl;

    i128 result = 0;
    for (const auto& part: inclusionExclusion) {
        if (part.first == 1) {
            continue;
        }

        i128 nTerms = limit/part.first;
        i128 sum = part.first*nTerms*(nTerms + 1)/2;
        result += sum*part.second;
    }

    cout << result << endl;
}

int main(int argc, char* argv[]) {
    solve(1000000LL);
    solve(1000000000LL);
    solve(kLimit);
    return 0;
}
