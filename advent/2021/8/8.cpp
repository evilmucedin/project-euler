#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

void first() {
    const auto input = readInputLines();

    size_t count = 0;
    for (const auto& s : input) {
        const auto parts1 = split(s, '|');
        trimAll(parts1);
        const auto parts2 = split(parts1[1], ' ');
        trimAll(parts2);
        for (const auto& w: parts2) {
            if (w.size() == 2 || w.size() == 3 || w.size() == 7 || w.size() == 4) {
                ++count;
            }
        }
    }

    cout << count << endl;
}

using S = IntVector;
using SS = vector<S>;

IntVector sToS(const string& s) {
    S res(128);
    for (char ch: s) {
        res[ch] = 1;
    }
    return res;
}

string STos(const S& s) {
    string res;
    for (int ch = 1; ch < 128; ++ch) {
        if (s[ch]) {
            res += ch;
        }
    }
    return res;
}

size_t len(const S& s) {
    size_t res = 0;
    for (auto i: s) {
        if (i) {
            ++res;
        }
    }
    return res;
}

S find(const SS& ss, size_t ln) {
    for (auto& s: ss) {
        if (len(s) == ln) {
            return s;
        }
    }
    throw Exception();
}

SS findAll(const SS& ss, size_t ln) {
    SS res;
    for (auto& s: ss) {
        if (len(s) == ln) {
            res.emplace_back(s);
        }
    }
    return res;
}

S minusS(const S& a, const S& b) {
    S res(128);
    for (size_t i = 0; i < 128; ++i) {
        if (a[i] && !b[i]) {
            res[i] = 1;
        }
    }
    return res;
}

S add(const S& a, const S& b) {
    S res(128);
    for (size_t i = 0; i < 128; ++i) {
        if (a[i] || b[i]) {
            res[i] = 1;
        }
    }
    return res;
}

S intersect(const S& a, const S& b) {
    S res(128);
    for (size_t i = 0; i < 128; ++i) {
        if (a[i] && b[i]) {
            res[i] = 1;
        }
    }
    return res;
}

SS remove(const SS& ss, const S& x) {
    SS res;
    for (const auto& s: ss) {
        if (s != x) {
            res.emplace_back(s);
        }
    }
    return res;
}

bool has(const S& s, const S& feat) {
    for (size_t i = 0; i < 128; ++i) {
        if (feat[i] && !s[i]) {
            return false;
        }
    }
    return true;
}

bool eq(const S& s, const S& feat) {
    for (size_t i = 0; i < 128; ++i) {
        if (feat[i] != s[i]) {
            return false;
        }
    }
    return true;
}

S findFeat(const SS& ss, const S& feat) {
    for (const auto& s: ss) {
        if (has(s, feat)) {
            return s;
        }
    }
    throw Exception();
}

S findNotFeat(const SS& ss, const S& feat) {
    for (const auto& s: ss) {
        if (!has(s, feat)) {
            return s;
        }
    }
    throw Exception();
}

size_t solve(const SS& ss, const string& str) {
    S s = sToS(str);
    for (size_t index = 0; index < 10; ++index) {
        if (eq(s, ss[index])) {
            return index;
        }
    }
    cerr << "Failed '" << str << "'" << endl;
    throw Exception();
}

void second() {
    const auto input = readInputLines();

    size_t sum = 0;
    for (const auto& s : input) {
        auto parts1 = split(s, '|');
        parts1 = removeEmpty(trimAll(parts1));

        auto parts3 = split(parts1[0], ' ');
        parts3 = removeEmpty(trimAll(parts3));

        SS ss(10);
        for (size_t i = 0; i < 10; ++i) {
            ss[i] = sToS(parts3[i]);
        }

        auto one = find(ss, 2);
        auto seven = find(ss, 3);
        auto four = find(ss, 4);
        auto eight = find(ss, 7);

        auto s235 = findAll(ss, 5);
        auto s690 = findAll(ss, 6);

        auto feat1 = minusS(four, one);
        auto feat2 = minusS(eight, add(add(one, seven), four));
        auto five = findFeat(s235, feat1);
        auto two = findFeat(s235, feat2);
        auto three = remove(remove(s235, two), five)[0];

        auto feat3 = intersect(five, two);
        auto nine = findNotFeat(s690, feat2);
        auto zero = findNotFeat(s690, feat3);
        auto six = remove(remove(s690, nine), zero)[0];

        SS sol = {zero, one, two, three, four, five, six, seven, eight, nine};
        /*
        for (size_t i = 0; i < 10; ++i) {
            cerr << i << " " << STos(sol[i]) << endl;
        }
        */

        auto parts2 = split(parts1[1], ' ');
        parts2 = removeEmpty(trimAll(parts2));
        // cerr << parts2 << endl;
        sum += solve(sol, parts2[0])*1000;
        sum += solve(sol, parts2[1])*100;
        sum += solve(sol, parts2[2])*10;
        sum += solve(sol, parts2[3])*1;
    }

    cout << sum << endl;
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

