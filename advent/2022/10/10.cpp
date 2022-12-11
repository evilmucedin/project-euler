#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    int x = 1;
    int result = 0;
    vector<int> xh;
    for (const auto& s : input) {
        const auto parts = split(s, ' ');
        if (parts[0] == "noop") {
            xh.emplace_back(x);
        } else {
            xh.emplace_back(x);
            xh.emplace_back(x);
            x += atoi(parts[1].c_str());
        }
    }
    for (int i = 20; i < 230; i += 40) {
        cerr << i << " " << xh[i + 1] << endl;
        result += i*xh[i - 1];
    }
    cout << result << endl;

    vector<vector<char>> m(6, vector<char>(40, '.'));
    auto sf = [](int x) {
        if (x < 0) {
            return 0;
        }
        if (x > 39) {
            return 39;
        }
        return x;
    };
    for (int i = 0; i < 40*6; ++i) {
        auto x = xh[i];
        if (((i % 40) >= x - 1 && (i % 40) <= x + 1)) {
            m[i / 40][i % 40] = '#';
        }
        // m[i / 40][sf(x % 40 + 1)] = '#';
        // m[i / 40][sf(x % 40 - 1)] = '#';
    }
    for (auto& s: m) {
        cerr << s << endl;
    }
    for (int i = 0; i < 6; ++i) {
        for (int j = 0; j < 40; ++j) {
            cerr << m[i][j];
        }
        cerr << endl;
    }
    // result = 20*xh[20] + 60*xh[60] + 100*xh[100] + 140*xh[140] + 180*xh[180] + 220*xh[220];
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

