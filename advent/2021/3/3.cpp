#include "advent/lib/aoc.h"
#include "gflags/gflags.h"
#include "lib/header.h"
#include "lib/init.h"

DEFINE_int32(test, 1, "test number");

IntVector countFreq(const StringVector& v) {
    const auto m = v[0].size();
    IntVector count(m);
    for (const auto& s : v) {
        for (size_t i = 0; i < m; ++i) {
            count[i] += s[i] == '1';
        }
    }
    return count;
}

void first() {
    const auto v = readInputLines();

    const auto n = v.size();
    const auto m = v[0].size();
    const auto count = countFreq(v);
    // cerr << count << " " << m << endl;

    IntVector gamma(m);
    IntVector eps(m);
    for (size_t i = 0; i < m; ++i) {
        if (2 * count[i] < n) {
            gamma[i] = 0;
            eps[i] = 1;
        } else {
            gamma[i] = 1;
            eps[i] = 0;
        }
    }

    // cerr << gamma << endl;
    // cerr << eps << endl;

    cout << digitsToNum(gamma, 2) * digitsToNum(eps, 2) << endl;
}

IntVector filter(int index, StringVector v) {
    const auto m = v[0].size();
    for (size_t i = 0; (i < m) && (v.size() > 1); ++i) {
        // cerr << v << endl;
        const auto count = countFreq(v);

        char target = 0;
        if (index == 0) {
            if (2 * count[i] == v.size()) {
                target = '1';
            } else {
                if (2 * count[i] > v.size()) {
                    target = '1';
                } else {
                    target = '0';
                }
            }
        } else {
            if (2 * count[i] == v.size()) {
                target = '0';
            } else {
                if (2 * count[i] > v.size()) {
                    target = '0';
                } else {
                    target = '1';
                }
            }
        }

        StringVector vv;
        for (const auto& s : v) {
            if (s[i] == target) {
                vv.emplace_back(s);
            }
        }
        v.swap(vv);
    }
    if (v.size() == 1) {
        IntVector iv;
        for (auto ch : v[0]) {
            iv.emplace_back(ch - '0');
        }
        // cerr << iv << endl;
        return iv;
    }
    throw std::exception();
}

void second() {
    const auto v = readInputLines();
    cout << digitsToNum(filter(0, v), 2) * digitsToNum(filter(1, v), 2) << endl;
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
