#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include <gmp.h>

#include "lib/header.h"
#include "lib/string.h"

#include "advent/lib/aoc.h"

using namespace std;

int main() {
    const auto lines = readInputLinesAll();
    int i = 0;
    // std::set<long long int> s;
    std::vector<pair<long long int, long long int>> ss;
    while (lines[i] != "") {
        const int n = lines[i].size();
        long long int a = 0;
        long long int j = 0;
        while (lines[i][j] != '-') {
            a = 10*a + (lines[i][j] - '0');
            ++j;
        }
        ++j;

        long long int b = 0;
        while (j < n) {
            b = 10*b + (lines[i][j] - '0');
            ++j;
        }

        cerr << a << " " << b << " " << (b - a) << endl;

        /*
        for (j = a; j <= b; ++j) {
            s.insert(j);
        }
        */
        ss.push_back( pair<long long int, long long int>(a, b) );

        cerr << a << " " << b << endl;

        ++i;
    }

    // cerr << "2: " << s.size() << endl;

    std::vector<pair<long long int, long long int>> ss2;
    for (const auto& x: ss) {
        int i = 0;
        while (i < ss2.size()) {
            if (ss2[i].first <= x.first && ss2[i].second <= x.second) {
                break;
            }
            ++i;
        }
        if (i >= ss2.size())
            ss2.push_back(x);
    }
    cerr << ss2 << endl;

    long long int sum2 = 0;
    for (const auto& x: ss2) {
        sum2 += x.second - x.first + 1;
    }
    cerr << "2: " << sum2 << endl;

    ++i;
    int sum = 0;

    cerr << i << " " << lines.size() << endl;
    while (i < lines.size()) {
        long long int x = stoll(lines[i]);
        cerr << x << endl;
        int j = 0;
        while (j != ss.size()) {
            if (x >= ss[j].first && x <= ss[j].second ) {
                ++sum;
                break;
            }
            ++j;
        }
        /*
        if (s.find(x) != s.end())
            ++sum;
        */
        ++i;
    }

    cout << sum << endl;

    return 0;
}
