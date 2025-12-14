#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "lib/header.h"
#include "lib/string.h"

#include "advent/lib/aoc.h"

using namespace std;

int main() {
    const auto lines = readInputLines();

    vector<pair<long long int, long long int>> x;
    for (const auto& s: lines) {
        int a = 0;
        int i = 0;
        while (s[i] != ',') {
            a = 10*a + (s[i] - '0');
            ++i;
        }
        ++i;
        int b = 0;
        while (i < s.size()) {
            b = 10*b + (s[i] - '0');
            ++i;
        }
        x.push_back({a, b});
    }
    cerr << x << endl;

    long long int sum = 0;
    for (int i = 0; i < x.size(); ++i)
        for (int j = i + 1; j < x.size(); ++j) {
                long long int a = x[i].first - x[j].first + 1;
                long long int b = x[i].second - x[j].second + 1;
                if (a < 0)
                    a = -a;
                if (b < 0)
                    b = -b;
                a = a*b;
                sum = max(sum, a);;
            }

    cout << sum << endl;

    return 0;
}
