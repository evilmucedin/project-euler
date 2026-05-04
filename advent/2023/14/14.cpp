#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    auto input = readInputLines();
    int result = 0;
    const int n = input.size();
    const int m = input[0].size();
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (input[i][j] == 'O') {
                int k = i;
                while (k >= 1 && input[k - 1][j] == '.') {
                    swap(input[k - 1][j], input[k][j]);
                    --k;
                }
                result += n - k;
            }
        }
    }
    // cerr << input << endl;
    cout << result << endl;
}

void second() {
    auto input = readInputLines();
    int result = 0;
    const int n = input.size();
    const int m = input[0].size();
    for (int cyc = 0; cyc < 10000; ++cyc) {
        static constexpr int DIRS[] = {-1, 0, 0, -1, 1, 0, 0, 1};
        int cyc4 = cyc % 4;

        auto g = [](char ch) { return ch == '.' || ch == 'O'; };

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (input[i][j] == 'O') {
                    if (cyc4 == 0) {
                        int k = i;
                        while (k >= 1 && g(input[k - 1][j])) {
                            swap(input[k - 1][j], input[k][j]);
                            --k;
                        }
                    } else if (cyc4 == 1) {
                        int k = j;
                        while (k >= 1 && g(input[i][k - 1])) {
                            swap(input[i][k - 1], input[i][k]);
                            --k;
                        }
                    } else if (cyc4 == 2) {
                        int k = i;
                        while (k + 1 < n && g(input[k + 1][j])) {
                            swap(input[k + 1][j], input[k][j]);
                            ++k;
                        }
                    } else {
                        int k = j;
                        while (k >= 1 && g(input[i][k + 1])) {
                            swap(input[i][k + 1], input[i][k]);
                            ++k;
                        }
                    }
                }
            }
        }

        LOG(INFO) << OUT(cyc);
        for (int i = 0; i < n; ++i) {
            LOG(INFO) << input[i];
        }
        LOG(INFO) << endl;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (input[i][j] == 'O') {
                result += n - i;
            }
        }
    }
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

