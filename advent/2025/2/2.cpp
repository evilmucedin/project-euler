#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>

#include "lib/header.h"
#include "lib/string.h"

#include "advent/lib/aoc.h"

using namespace std;

bool isID(unsigned long long int x) {
    const string s = to_string(x);
    const auto len = s.size();
    if (len % 2 != 0)
        return false;
    auto j = len - len/2;
    for (int i = 0; i < len/2; ++i) {
        if (s[i] != s[j]) {
            // cerr << " " << x << " " << i << " " << j << " " << len << " " << s[i] << s[j] << endl;
            return false;
        }
        ++j;
    }
    // cerr << x << " " << len << endl;
    return true;
}

bool isID2(unsigned long long int x) {
    const string s = to_string(x);
    const auto len = s.size();
    for (int i = 1; i < len; ++i) {
        if (len % i == 0) {
            string ss;
            for (int j = 0; j < len / i; ++j)
                ss += s.substr(0, i);
            if (ss == s) {
                // cerr << x << endl;
                return true;
            } else {
                // cerr << s << " " << ss << " " << i << endl;
            }
        }
    }
    // cerr << x << endl;
    return false;
}

int main() {
    const auto lines = readInputLines();

    unsigned long long int result = 0;
    unsigned long long int ans = 0;

    for (const string& s: lines) {
        cerr << s << endl;
        unsigned long long int a = 0;
        unsigned long long int b = 0;

        int i = 0;
        while (i < s.size()) {
            int x = 0;
            while (s[i] != '-') {
                a = 10*a + (s[i] - '0');
                ++i;
            }
            ++i;
            while (i < s.size() && s[i] != ',') {
                b = 10*b + (s[i] - '0');
                ++i;
            }

            cerr << a << " " << b << endl;
            result += b - a;

            for (unsigned long long i = a; i <= b; ++i)
                if (isID2(i)) {
                    ans += i;
                    ++x;
                }

            cerr << a << " " << b << " " << x << endl;

            a = 0;
            b = 0;

            if (s[i] == ',')
                ++i;
        }

        // result += b - a;
    }

    cout << ans << endl;

    return 0;
}
