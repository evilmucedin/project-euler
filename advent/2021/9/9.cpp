#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "lib/init.h"
#include "lib/string.h"

DEFINE_int32(test, 1, "test number");

static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};

void first() {
    const auto input = readInputLines();
    size_t sum = 0;
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            bool good = true;
            for (int k = 0; k < 4; ++k) {
                int ii = i + DIRS[2 * k];
                int jj = j + DIRS[2 * k + 1];
                if (ii >= 0 && ii < input.size()) {
                    if (jj >= 0 && jj < input[i].size()) {
                        if (input[ii][jj] <= input[i][j]) {
                            good = false;
                        }
                    }
                }
            }
            if (good) {
                sum += input[i][j] - '0' + 1;
            }
        }
    }
    cout << sum << endl;
}

int dfs(int x, int y, StringVector& s) {
    if (x < 0 || x >= s.size()) {
        return 0;
    }
    if (y < 0 || y >= s[0].size()) {
        return 0;
    }
    if (s[x][y] == 0) {
        return 0;
    }
    if (s[x][y] == '9') {
        return 0;
    }
    int res = 0;
    s[x][y] = 0;
    ++res;
    for (int k = 0; k < 4; ++k) {
        res += dfs(x + DIRS[2 * k], y + DIRS[2 * k + 1], s);
    }
    return res;
}

void second() {
    const auto input = readInputLines();
    auto input2 = input;
    IntVector sizes;
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            bool good = true;
            for (int k = 0; k < 4; ++k) {
                int ii = i + DIRS[2 * k];
                int jj = j + DIRS[2 * k + 1];
                if (ii >= 0 && ii < input.size()) {
                    if (jj >= 0 && jj < input[i].size()) {
                        if (input[ii][jj] <= input[i][j]) {
                            good = false;
                        }
                    }
                }
            }
            if (good) {
                sizes.emplace_back(dfs(i, j, input2));
            }
        }
    }
    sort(sizes);
    reverse(sizes);
    cout << sizes[0] * sizes[1] * sizes[2] << endl;
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
