#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Item {
    StringVector field;
    int d{};

    bool operator<(const Item& rhs) const {
        return d > rhs.d;
    }

    bool operator>(const Item& rhs) const {
        return d < rhs.d;
    }
};

void first() {
    const auto input = readInputLines();

    using Q = priority_queue<Item>;
    using H = unordered_map<StringVector, int>;

    Q q;
    q.push( {input, 0} );

    H h;

    int step = 0;
    while (!q.empty() && (step < 10000000)) {
        const auto now = q.top();
        q.pop();
        auto toNow = h.find(now.field);
        const auto& f = now.field;
        if (toNow == h.end() || (toNow->second > now.d)) {
            REP (i, f.size()) {
                REP (j, f[i].size()) {
                    char ch = f[i][j];
                    if (ch == 'A' || ch == 'B' || ch == 'C' || ch == 'D') {
                        static const int DIRS[] = {-1, 0, 1, 0, 0, -1, 0, 1};
                        REP (k, 4) {
                            for (int l = -10; l <= 10; ++l) {
                                int ii = i + l * DIRS[2 * k];
                                int jj = j + l * DIRS[2 * k + 1];
                                if (ii >= 0 && ii < f.size()) {
                                    if (jj >= 0 && jj < f[ii].size()) {
                                        if (f[ii][jj] == '.') {
                                            if (ii != 1 || !(jj == 3 || jj == 5 || jj == 7 || jj == 9)) {

                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    cout << endl;
}

void second() {
    const auto input = readInputLines();
    cout << endl;
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

