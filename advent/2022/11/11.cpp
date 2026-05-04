#include <functional>

#include "advent/lib/aoc.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int result = 0;

    std::vector<std::function<int(int)>> op;

    /*
    const int n = 4;
    vector<vector<int>> w = {{79, 98}, {54, 65, 75, 74}, {79, 60, 97}, {74}};

    auto l1 = [](int old) -> int { return old * 19; };
    auto l2 = [](int old) -> int { return old + 6; };
    auto l3 = [](int old) -> int { return old * old; };
    auto l4 = [](int old) -> int { return old + 3; };

    op.push_back(l1);
    op.push_back(l2);
    op.push_back(l3);
    op.push_back(l4);
    vector<int> test = {23, 19, 13, 17};
    vector<int> testTrue = {2, 2, 1, 0};
    vector<int> testFalse = {3, 0, 3, 1};
    */

    const int n = 8;
    vector<vector<int>> w = {{96, 60, 68, 91, 83, 57, 85}, {75, 78, 68, 81, 73, 99}, {69, 86, 67, 55, 96, 69, 94, 85}, {88, 75, 74, 98, 80}, {82}, {72, 92, 92}, {74, 61}, {76, 86, 83, 55}};
    auto l0 = [](int old) -> int { return old * 2; };
    auto l1 = [](int old) -> int { return old + 3; };
    auto l2 = [](int old) -> int { return old + 6; };
    auto l3 = [](int old) -> int { return old + 5; };
    auto l4 = [](int old) -> int { return old + 8; };
    auto l5 = [](int old) -> int { return old * 5; };
    auto l6 = [](int old) -> int { return old * old; };
    auto l7 = [](int old) -> int { return old + 4; };
    op.push_back(l0);
    op.push_back(l1);
    op.push_back(l2);
    op.push_back(l3);
    op.push_back(l4);
    op.push_back(l5);
    op.push_back(l6);
    op.push_back(l7);

    vector<int> test = {17, 13, 19, 7, 11, 3, 2, 5};
    vector<int> testTrue = {2, 7, 6, 7, 0, 6, 3, 4};
    vector<int> testFalse = {5, 4, 5, 1, 2, 3, 1, 0};

    int md = 1;
    for (auto t: test) {
        md = md*t / __gcd(md, t);
    }
    cerr << "MD: " << md << endl;

    vector<i64> ops(n);

    for (int it = 0; it < 10000; ++it) {
        LOG_EVERY_MS(INFO, 1000) << it;

        vector<vector<int>> wNew(n);

        for (int i = 0; i < n; ++i) {
            for (auto it = w[i].begin(); it != w[i].end(); ++it) {
                ++ops[i];
                auto ww = *it;
                ww = (op[i](ww) / 3) % (9699690);
                if ((ww % test[i]) == 0) {
                    w[testTrue[i]].push_back(ww);
                } else {
                    w[testFalse[i]].push_back(ww);
                }
            }
            w[i].clear();
        }

        // w = wNew;
        /*
        for (int i = 0; i < n; ++i) {
            cerr << w[i] << endl;
        }
        cerr << endl;
        */
    }

    sort(ops.begin(), ops.end());
    result = ops[n - 2] * ops[n - 1];

    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    i64 result = 0;

    std::vector<std::function<int(int)>> op;

    const int n = 4;
    vector<vector<i64>> w = {{79, 98}, {54, 65, 75, 74}, {79, 60, 97}, {74}};

    auto l1 = [](i64 old) -> i64 { return old * 19; };
    auto l2 = [](i64 old) -> i64 { return old + 6; };
    auto l3 = [](i64 old) -> i64 { return old * old; };
    auto l4 = [](i64 old) -> i64 { return old + 3; };

    op.push_back(l1);
    op.push_back(l2);
    op.push_back(l3);
    op.push_back(l4);
    vector<i64> test = {23, 19, 13, 17};
    vector<int> testTrue = {2, 2, 1, 0};
    vector<int> testFalse = {3, 0, 3, 1};
    static constexpr int MD = 96577*3*2*5*7;
    /*
    */

    /*
    const int n = 8;
    vector<vector<i64>> w = {{96, 60, 68, 91, 83, 57, 85}, {75, 78, 68, 81, 73, 99}, {69, 86, 67, 55, 96, 69, 94, 85}, {88, 75, 74, 98, 80}, {82}, {72, 92, 92}, {74, 61}, {76, 86, 83, 55}};
    auto l0 = [](i64 old) -> i64 { return old * 2; };
    auto l1 = [](i64 old) -> i64 { return old + 3; };
    auto l2 = [](i64 old) -> i64 { return old + 6; };
    auto l3 = [](i64 old) -> i64 { return old + 5; };
    auto l4 = [](i64 old) -> i64 { return old + 8; };
    auto l5 = [](i64 old) -> i64 { return old * 5; };
    auto l6 = [](i64 old) -> i64 { return old * old; };
    auto l7 = [](i64 old) -> i64 { return old + 4; };
    op.push_back(l0);
    op.push_back(l1);
    op.push_back(l2);
    op.push_back(l3);
    op.push_back(l4);
    op.push_back(l5);
    op.push_back(l6);
    op.push_back(l7);

    vector<int> test = {17, 13, 19, 7, 11, 3, 2, 5};
    vector<int> testTrue = {2, 7, 6, 7, 0, 6, 3, 4};
    vector<int> testFalse = {5, 4, 5, 1, 2, 3, 1, 0};
    static constexpr int MD = 9699690;
    */

    i64 md = 1;
    for (auto t: test) {
        md = md*t / __gcd(md, t);
    }
    cerr << "MD: " << md << ", MD: " << MD << endl;

    vector<i64> ops(n);

    for (int it = 0; it < 10000; ++it) {
        LOG_EVERY_MS(INFO, 1000) << it;

        for (int i = 0; i < n; ++i) {
            for (auto it = w[i].begin(); it != w[i].end(); ++it) {
                ++ops[i];
                auto ww = *it;
                ww = op[i](ww) % MD;
                if ((ww % test[i]) == 0) {
                    w[testTrue[i]].push_back(ww);
                } else {
                    w[testFalse[i]].push_back(ww);
                }
            }
            w[i].clear();
        }

        // w = wNew;
        /*
        for (int i = 0; i < n; ++i) {
            cerr << w[i] << endl;
        }
        cerr << endl;
        */
    }

    cerr << ops << endl;
    sort(ops.begin(), ops.end());
    result = ops[n - 2] * ops[n - 1];

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
