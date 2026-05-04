#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

vector<long long> mapSeeds(const vector<long long>& seeds, const vector<string>& input) {
    int k = 0;
    auto prev = seeds;
    for (int i = 0; i < 7; ++i) {
        while (input[k].find("map") == string::npos) {
            ++k;
        }
        ++k;

        vector<long long> next;
        multiset<long long> prevCopy(prev.begin(), prev.end());
        while (k < input.size() && trim(input[k]) != "") {
            const auto intv = parseIntegers<long long>(input[k]);
            ++k;
            for (auto i: prev) {
                if (i >= intv[1] && i < intv[1] + intv[2]) {
                    next.emplace_back(i - intv[1] + intv[0]);
                    prevCopy.erase(i);
                }
            }
        }
        for (auto i: prevCopy) {
            next.emplace_back(i);
        }

        swap(prev, next);
        // LOG(INFO) << OUT(i) << OUT(prev);
    }

    return prev;
}

template<typename T>
int minIndex(const T& v) {
    int i = 0;
    int mn = v[0];
    for (int j = 1; j < v.size(); ++j) {
        if (v[j] < mn) {
            mn = v[j];
            i = j;
        }
    }
    return i;
}

void first() {
    const auto input = readInputLinesAll();
    const auto seeds = parseIntegers<long long>(input[0]);
    cout << OUT(minV(mapSeeds(seeds, input)));
}

void second() {
    const auto input = readInputLinesAll();
    const auto seeds = parseIntegers<long long>(input[0]);

    long long result2 = 1ULL << 40;
    for (int k = 0; k < seeds.size(); k += 2) {
        long long x = seeds[k];
        for (int t = 0; t < 100; ++t) {
            LOG(INFO) << OUT(k) << OUT(t) << OUT(x) << OUT(result2);
            auto oldx = x;
            for (long long j = -10000; j < 10000; ++j)
                for (long long m : {1, 100, 10000, 1000000, 1000000000}) {
                    if (x + m*j >= seeds[k] && x + m*j < seeds[k] + seeds[k + 1]) {
                        const auto vals = mapSeeds({x + m*j}, input);
                        if (vals[0] < result2) {
                            result2 = vals[0];
                            x = x + m*j;
                        }
                    }
                }
            if (oldx == x) {
                break;
            }
        }
    }

    LOG(INFO) << OUT(result2);
}

void second_() {
    const auto input = readInputLinesAll();
    const auto seeds = parseIntegers<long long>(input[0]);

    vector<long long> cand;
    for (int i = 0; i < seeds.size(); i += 2) {
        cand.emplace_back(seeds[i]);
        for (int j = 0; j < 1000; ++j) {
            cand.emplace_back(seeds[i] + (seeds[i + 1]*j/1000));
        }
        if (seeds[i + 1] < 1000000) {
            for (int j = 0; j < seeds[i + 1]; ++j) {
                cand.emplace_back(seeds[i] + j);
            }
        }
    }

    for (int k = 0; k < seeds.size(); k += 2) {
        for (int t = 0; t < 10; ++t) {
            const auto mapped = mapSeeds(cand, input);
            int result = minIndex(mapped);
            LOG(INFO) << OUT(result) << OUT(cand[result]) << endl;
            cout << OUT(mapped[result]) << OUT(mapSeeds({cand[result]}, input)) << endl;

            LOG(INFO) << OUT(seeds[k]) << OUT(seeds[k] + seeds[k + 1]);

            long long x = cand[result];

            for (long long j = -10000; j < 10000; ++j) {
                if (x + j >= seeds[k] && x + j < seeds[k] + seeds[k + 1]) {
                    cand.emplace_back(x + j);
                }
                if (x + j * 100 >= seeds[k] && x + j * 100 < seeds[k] + seeds[k + 1]) {
                    cand.emplace_back(x + j * 100);
                }
            }

            while (x + 1 < seeds[k] + seeds[k + 1] && mapSeeds({x}, input)[0] > mapSeeds({x + 1}, input)[0]) {
                ++x;
            }
            cand.emplace_back(x);
            LOG(INFO) << OUT(x) << OUT(cand[result]) << OUT(mapSeeds({x}, input)) << OUT(minV(mapSeeds(cand, input)));
        }
    }
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}

