#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

void check(const StringVector& v) {
    for (const auto& s: v) {
        for (auto ch: s) {
            if (ch != '#' && ch != '.') {
                throw Exception();
            }
        }
    }
}

void first() {
    auto input = removeEmpty(readInputLinesAll());
    check(input);

    const string pattern = input[0];

    size_t n = input.size() - 1;
    size_t m = input[1].size();
    cerr << n << "x" << m << "x" << pattern.size() << endl;
    IntMatrix img(n, IntVector(m));
    REP (i, n) {
        if (input[i + 1].size() != m) {
            throw Exception("in");
        }
        REP (j, m) {
            img[i][j] = (input[i + 1][j] == '#') ? 1 : 0;
        }
    }

    static constexpr size_t PAD = 30;
    REP (i, n) {
        REP (j, PAD) {
            img[i].insert(img[i].begin(), 0);
        }
        REP (j, PAD) {
            img[i].insert(img[i].end(), 0);
        }
    }
    const IntVector pad(m + 2*PAD);
    REP(_, PAD) { img.insert(img.begin(), pad); }
    REP(_, PAD) { img.insert(img.end(), pad); }

    n += 2 * PAD;
    m += 2 * PAD;

    IntMatrix oldImg = img;

    auto get = [&](int i, int j) {
        ASSERTEQ(oldImg.size(), n);
        if (i >= 0 && i < n) {
            ASSERTEQ(oldImg[i].size(), m);
            if (j >= 0 && j < m) {
                return oldImg[i][j];
            }
        }
        return 0;
    };

    auto print = [&]() {
        REP (i, n) {
            REP(j, m) {
                cerr << (img[i][j] ? '#' : '.');
            }
            cerr << endl;
        }
        cerr << endl;
    };

    REP (_, 2) {
        // print();
        oldImg = img;
        REP (i, n) {
            REP (j, m) {
                int code = 0;
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        code = (code << 1) + get(i + di, j + dj);
                    }
                }
                assert(code < pattern.size());
                // cerr << i << " " << j << " " << code << endl;
                img[i][j] = pattern[code] == '#';
            }
        }
    }
    // print();

    int res = 0;
    REP(i, n) {
        if (i == 0 || i + 1 == n) {
            continue;
        }
        REP(j, m) {
            if (j == 0 || j + 1 == m) {
                continue;
            }
            res += img[i][j];
        }
    }

    cout << "1: " << res << endl;
}

void second() {
    auto input = removeEmpty(readInputLinesAll());
    check(input);

    const string pattern = input[0];

    size_t n = input.size() - 1;
    size_t m = input[1].size();
    cerr << n << "x" << m << "x" << pattern.size() << endl;
    IntMatrix img(n, IntVector(m));
    REP (i, n) {
        if (input[i + 1].size() != m) {
            throw Exception("in");
        }
        REP (j, m) {
            img[i][j] = (input[i + 1][j] == '#') ? 1 : 0;
        }
    }

    static constexpr size_t PAD = 500;
    REP (i, n) {
        REP (j, PAD) {
            img[i].insert(img[i].begin(), 0);
        }
        REP (j, PAD) {
            img[i].insert(img[i].end(), 0);
        }
    }
    const IntVector pad(m + 2*PAD);
    REP(_, PAD) { img.insert(img.begin(), pad); }
    REP(_, PAD) { img.insert(img.end(), pad); }

    n += 2 * PAD;
    m += 2 * PAD;

    IntMatrix oldImg = img;

    auto get = [&](int i, int j) {
        ASSERTEQ(oldImg.size(), n);
        if (i >= 0 && i < n) {
            ASSERTEQ(oldImg[i].size(), m);
            if (j >= 0 && j < m) {
                return oldImg[i][j];
            }
        }
        return 0;
    };

    auto print = [&]() {
        REP (i, n) {
            REP(j, m) {
                cerr << (img[i][j] ? '#' : '.');
            }
            cerr << endl;
        }
        cerr << endl;
    };

    static constexpr size_t IT = 50;
    REP (_, IT) {
        // print();
        oldImg = img;
        REP (i, n) {
            REP (j, m) {
                int code = 0;
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        code = (code << 1) + get(i + di, j + dj);
                    }
                }
                assert(code < pattern.size());
                // cerr << i << " " << j << " " << code << endl;
                img[i][j] = pattern[code] == '#';
            }
        }
    }
    // print();

    int res = 0;
    REP(i, n) {
        if (i < IT || i + IT > n) {
            continue;
        }
        REP(j, m) {
            if (j < IT || j + IT > m) {
                continue;
            }
            res += img[i][j];
        }
    }

    cout << "2: " << res << endl;
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

