#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

int dfs(const StringVector& f, int x, int y, const string& target, int pos, set<pair<int, int>>& used) {
    if (x < 0 || y < 0) {
        return 0;
    }
    if (x >= f.size() || y >= f[x].size()) {
        return 0;
    }
    if (f[x][y] != target[pos]) {
        return 0;
    }
    if (pos == target.size()) {
        return 1;
    }
    pair<int, int> now(x, y);
    int result = 0;
    if (f[x][y] == target[pos] && used.find(now) == used.end()) {
        used.emplace(now);
        const int DIRS[] = {-1, 0, 0, -1, 0, 1, 1, 0, 1, 1, 1, -1, -1, 1, -1, -1};
        for (int k = 0; k < 8; ++k) {
            int nx = x + DIRS[2*k];
            int ny = y + DIRS[2 * k + 1];
            result += dfs(f, nx, ny, target, pos + 1, used);
        }
        used.erase(now);
    }
    return result;
}

void first() {
    const auto input = readInputLines();
    int result = 0;
    set<pair<int, int>> used;
    for (int i = 0; i < input.size(); ++i) {
        for (int j = 0; j < input[i].size(); ++j) {
            result += dfs(input, i, j, "XMAS", 0, used);
        }
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

