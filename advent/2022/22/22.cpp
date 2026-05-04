#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLinesAll();

    vector<string> f;
    for (int i = 0; i + 2 < input.size(); ++i) {
        f.emplace_back(input[i]);
    }

    const int n = f.size();
    int m = input[0].size();
    for (int i = 1; i < n; ++i) {
        m = max<int>(m, input[i].size());
    }
    for (auto& s: f) {
        while (s.size() < m) {
            s += ' ';
        }
    }
    auto cf = f;

    cerr << "(" << n << ", " << m << ")" << endl;

    vector<string> cmds;
    int begin = 0;
    for (int i = 0; i < input.back().size(); ++i) {
        if (input.back()[i] == 'L' || input.back()[i] == 'R') {
            if (begin < i) {
                cmds.emplace_back(input.back().substr(begin, i - begin));
            }
            cmds.emplace_back(string(1, input.back()[i]));
            begin = i + 1;
        }
    }
    if (begin < input.back().size()) {
        cmds.emplace_back(input.back().substr(begin, input.back().size() - begin));
    }

    cerr << cmds << endl;
    cerr << join("", cmds) << endl;

    int x = 0;
    int y = 0;
    while (f[x][y] != '.') {
        ++y;
    }

    static const int DIRS[] = {0, 1, 1, 0, 0, -1, -1, 0};
    static const char DIRC[] = {'>', 'v', '<', '^'};

    int iDir = 0;

    auto wall = [&](int x, int y) {
        if (x < 0 || x >= n) {
            return false;
        }
        if (y < 0 || y >= f[x].size()) {
            return false;
        }
        return f[x][y] == '#';
    };

    auto good = [&](int x, int y) {
        if (x < 0 || x >= n) {
            return false;
        }
        if (y < 0 || y >= f[x].size()) {
            return false;
        }
        return f[x][y] == '.';
    };

    for (const auto& cmd : cmds) {
        if (wall(x, y)) {
            THROW("Wall");
        }
        cerr << cmd << " [" << x << ", " << y << "] " << iDir << endl;
        if (cmd == "R") {
            iDir = (iDir + 1) % 4;
        } else if (cmd == "L") {
            iDir = (iDir + 3) % 4;
        } else {
            int mv = atoi(cmd.c_str());
            for (int i = 0; i < mv; ++i) {
                cf[x][y] = DIRC[iDir];
                int nx = (x + DIRS[2 * iDir]) % n;
                int ny = (y + DIRS[2 * iDir + 1]) % m;
                if (wall(nx, ny)) {
                    break;
                }
                if (!good(nx, ny)) {
                    int j = 0;
                    while (!good(nx, ny) && (j < m + n + m + n)) {
                        nx = (nx + DIRS[2 * iDir]) % n;
                        ny = (ny + DIRS[2 * iDir + 1]) % m;
                        if (wall(nx, ny)) {
                            break;
                        }
                        ++j;
                    }
                }
                if (good(nx, ny)) {
                    x = nx;
                    y = ny;
                } else {
                    break;
                }
            }
            cf[x][y] = DIRC[iDir];
        }
    }

    for (auto& s: cf) {
        cerr << s << endl;
    }
    cerr << endl;

    cerr << OUT(x) << OUT(y) << OUT(iDir) << endl;
    int result = 1000 * (x + 1) + 4 * (y + 1) + iDir;
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

