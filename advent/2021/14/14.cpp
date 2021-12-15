#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

unordered_map<int, char> readPatterns(const StringVector& input) {
    unordered_map<int, char> patterns;

    for (size_t iLine = 2; iLine < input.size(); ++iLine) {
        patterns[128*input[iLine][0] + input[iLine][1]] = input[iLine][6];
    }

    return patterns;
}

void first() {
    const auto input = readInputLinesAll();

    string s = input[0];
    const auto patterns = readPatterns(input);

    REP (it, 10) {
        string res;
        REP (i, s.size()) {
            if (i + 1 < s.size()) {
                int key = 128 * s[i] + s[i + 1];
                auto toKey = patterns.find(key);
                if (toKey != patterns.end()) {
                    res += s[i];
                    res += toKey->second;
                } else {
                    res += s[i];
                }
            } else {
                res += s[i];
            }
        }
        s = res;
        // cerr << s << endl;
    }

    unordered_map<int, int> count;
    for (auto ch: s) {
        ++count[ch];
    }

    int mn = 10000000;
    int mx = -10000000;
    for (const auto& p: count) {
        if (p.second < mn) {
            mn = p.second;
        }
        if (p.second > mx) {
            mx = p.second;
        }
    }

    cout << mx - mn << endl;
}

void second() {
    const auto input = readInputLinesAll();

    string s = input[0];

    const auto patterns = readPatterns(input);

    using Counts = unordered_map<int, i64>;

    Counts counts;
    for (int i = 1; i < s.size(); ++i) {
        ++counts[128*s[i - 1] + s[i]];
    }

    REP (it, 40) {
        Counts newCounts;
        for (const auto& p: counts) {
            if (patterns.count(p.first)) {
                const auto toP = patterns.find(p.first);
                newCounts[(p.first / 128)*128 + toP->second] += p.second;
                newCounts[toP->second*128 + (p.first % 128)] += p.second;
            } else {
                newCounts[p.first] += p.second;
            }
        }
        counts.swap(newCounts);
    }

    Counts chCounts;
    for (const auto& p: counts) {
        chCounts[p.first / 128] += p.second;
        chCounts[p.first % 128] += p.second;
    }
    ++chCounts[s.front()];
    ++chCounts[s.back()];

    i64 mn = 1000000000000000;
    i64 mx = -1000000000000000;
    for (const auto& p: chCounts) {
        if (p.second < mn) {
            mn = p.second;
        }
        if (p.second > mx) {
            mx = p.second;
        }
    }

    cout << (mx - mn) / 2 << endl;
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

