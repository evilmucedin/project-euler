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

    int sum = 0;
    for (const string& s: lines) {
        const int l = s.size();
        int x = 0;
        for (int i = 0; i < l; ++i)
            for (int j = i + 1; j < l; ++j) {
                int y = (s[i] - '0') * 10 + (s[j] - '0');
                x = max(x, y);
            }
        sum += x;
    }

    cout << sum << endl;

    vector<string> lines2;
    for (string& s: lines) {
        cerr << s << " ";
        while (s.size() > 12) {
            mpz_t min;
            // mpz_set_str(min, s.c_str(), 10);
            mpz_set_ui(min, 0);
            mpz_t ssmpz;
            string minSS;
            for (int i = 0; i < s.size(); ++i) {
                string ss = s;
                ss.erase(i, 1);
                mpz_set_str(ssmpz, ss.c_str(), 10);

                /*
                cerr << "-";
                mpz_out_str(stderr, 10, min);
                cerr << " ";
                mpz_out_str(stderr, 10, ssmpz);
                cerr << endl;
                */

                if (mpz_cmp(ssmpz, min) > 0) {
                    // min = ssmpz;
                    mpz_set_str(min, ss.c_str(), 10);
                    minSS = ss;
                    cerr << " ";
                    mpz_out_str(stderr, 10, min);
                    cerr << " ";
                    mpz_out_str(stderr, 10, ssmpz);
                    cerr << endl;
                }
            }
            s = minSS;
            // lines2.push_back(minSS);
            mpz_set(min, ssmpz);
        }

/*
            char min = '9';
            int minX = s.size() - 1;
            for (int i = s.size() - 1; i >= 0; --i) {
                if (s[i] < min) {
                    min = s[i];
                    minX = i;
                } else if (s[i] == min) {
                    minX = i;
                }
            }
            s.erase(minX, 1);
        }
*/
        cerr << s << endl;
        lines2.push_back(s);
    }

    cerr << lines2.size() << endl;

    vector<int> sum2(lines2[0].size() + 10);
    for (const string& s: lines2) {
        const int l = s.size();
        int x = 0;
        for (int i = 0; i < l; ++i) {
            sum2[l - i - 1] += s[i] - '0';
        }
    }
    for (int i = 0; i < sum2.size(); ++i)
        cerr << sum2[i] << " ";
    cerr << endl;

    for (int x = 0; x < 10; ++x) {
        for (int i = sum2.size() - 2; i >= 0; --i) {
            sum2[i + 1] += sum2[i] / 10;
            sum2[i] %= 10;
        }
    }

    for (int i = 0; i < sum2.size(); ++i)
        cerr << sum2[i] << " ";
    cerr << endl;

    for (int i = sum2.size() - 1; i >= 0; --i)
        cout << sum2[i];

    cout << endl;


    return 0;
}
