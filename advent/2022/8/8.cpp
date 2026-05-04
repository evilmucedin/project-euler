#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();
    const int n = input.size();
    const int m = input[0].size();
    cerr << n << " " << m << endl;
    vector<vector<int>> visible(n, vector<int>(m, -1));
    int result = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (visible[i][j] == -1) {
                if (i == 0 || i + 1 == n || j == 0 || j + 1 == m) {
                    visible[i][j] = 1;
                    ++result;
                }
            }
        }
    }

    i64 mx = 0;
    for (int k = 0; k < 5; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < m; ++j) {
                if (visible[i][j] == -1) {

                    auto test = [&](int x0, int y0, int dx, int dy) {
                        int x = x0;
                        int y = y0;
                        while (x != 0 && x + 1 != n && y != 0 && y + 1 != m) {
                            x += dx;
                            y += dy;
                            if (x >= 0 && x < n && y >= 0 && y < m && (input[x][y] >= input[x0][y0])) {
                                return false;
                            }
                        }
                        return x == 0 || x + 1 == n || y == 0 || y + 1 == m;
                    };

                    auto testCount = [&](int x0, int y0, int dx, int dy) -> i64 {
                        int x = x0;
                        int y = y0;
                        int count = 0;
                        while (x != 0 && x + 1 != n && y != 0 && y + 1 != m) {
                            x += dx;
                            y += dy;
                            ++count;
                            if (x >= 0 && x < n && y >= 0 && y < m && (input[x][y] >= input[x0][y0])) {
                                return count;
                            }
                        }
                        return count;
                    };

                    if (test(i, j, 1, 0)) {
                        visible[i][j] = 1;
                        ++result;
                    } else if (test(i, j, 0, 1)) {
                        visible[i][j] = 1;
                        ++result;
                    } else if (test(i, j, -1, 0)) {
                        visible[i][j] = 1;
                        ++result;
                    } else if (test(i, j, 0, -1)) {
                        visible[i][j] = 1;
                        ++result;
                    }

                    mx = max(mx, testCount(i, j, 1, 0) * testCount(i, j, -1, 0) * testCount(i, j, 0, 1) *
                                     testCount(i, j, 0, -1));
                }
            }
        }
    }
    // cerr << visible << endl;

    cout << result << endl;
    cout << "mx:" << mx << endl;
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

