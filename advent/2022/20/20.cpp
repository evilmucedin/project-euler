#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    vector<pair<int, int>> nums;
    int index = 0;
    for (const auto& s: input) {
        nums.emplace_back(make_pair(atoi(s.c_str()), index));
        ++index;
    }
    cerr << nums.size() << endl;

    int result = 0;
    const int n = nums.size();
    for (int i = 0; i < n; ++i) {
        int k = i;
        while (nums[k].second != i) {
            k = (k + 1) % n;
        }
        int pos = (nums[k].first + (n - 1) * 10 + k) % (n - 1);
        int off = (pos < k) ? -1 : 0;
        auto tmp = nums[k];
        nums.erase(nums.begin() + k);
        nums.insert(nums.begin() + (pos + off + n - 1) % (n - 1), tmp);
        // cerr << OUT(k) << OUT(tmp) << OUT(pos) << OUT(off) << " " << nums << endl;
    }

    int p0 = 0;
    while (nums[p0].first != 0) {
        ++p0;
    }

    cerr << p0 << endl;
    cerr << nums[(p0 + 1000) % n] << " " <<  nums[(p0 + 2000) % n] << " " <<  nums[(p0 + 3000) % n] << endl;

    result = nums[(p0 + 1000) % n].first + nums[(p0 + 2000) % n].first + nums[(p0 + 3000) % n].first;

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

