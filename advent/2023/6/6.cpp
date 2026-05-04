#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLinesAll();
    const auto times = parseIntegers<long long>(input[0]);
    const auto ds = parseIntegers<long long>(input[1]);
    LOG(INFO) << OUT(times) << OUT(ds);
    long long result = 1;
    vector<long long> ways;
    for (int i = 0; i < times.size(); ++i) {
        int iways = 0;
        for (long long j = 1; j <= times[i]; ++j) {
            long long d = (times[i] - j)*j;
            if (d > ds[i]) {
                ++iways;
            }
        }
        ways.emplace_back(iways);
    }
    LOG(INFO) << OUT(ways);
    for (long long w: ways) {
        result *= w;
    }
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
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

