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
    auto lines = readInputLines();

    vector<vector<long long int>> v;
    for (int i = 0; i + 1 < lines.size(); ++i) {
        v.push_back({});
        long long int x = 0;
        for (char ch: lines[i]) {
            if (ch >= '0' && ch <= '9')
                x = 10*x + (ch - '0');
            else {
                if (x)
                    v[i].push_back(x);
                x = 0;
            }
        }
        if (x)
            v[i].push_back(x);
    }

    cerr << v << endl;

    long long int sum = 0;
    int vv = 0;
    for (int i = 0; i < lines[lines.size() - 1].size(); ++i) {
        if (lines[lines.size() - 1][i] == '+') {
            long long int sum2 = 0;
            for (int j = 0; j < v.size(); ++j)
                sum2 += v[j][vv];
            ++vv;
            sum += sum2;
        }
        if (lines[lines.size() - 1][i] == '*') {
            long long int sum2 = 1;
            for (int j = 0; j < v.size(); ++j)
                sum2 *= v[j][vv];
            ++vv;
            sum += sum2;
        }
        cerr << " " << i << " " << sum << endl;
    }

    cout << sum << endl;

    return 0;
}
