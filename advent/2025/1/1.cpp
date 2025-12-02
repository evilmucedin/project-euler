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

    int x = 50;
    int b = 0;
    int c = 0;
    for (const string& line: lines) {
        cerr << line << endl;
        int i = 0;
        for (int z = 1; z < line.size(); ++z) {
            if (line[z] >= '0' && line[z] <= '9')
                i = 10*i + (line[z] - '0');
            else
                raise 2;
        }

        // cerr << i << endl;
        if (line[0] == 'R') {
            for (int j = 0; j < i; ++i) {
                ++x;
                if (x == 100) {
                    ++c;
                    x -= 100;
                }
            }
        }
        else if (line[0] == 'L') {
            for (int j = 0; j < i; ++j) {
                --x;
                if (x < 0)
                    x += 100;
                if (x == 0) {
                    ++c;
                }
            }
        }
        else
            throw 1;
        while (x < 0) {
            x += 100;
        }
        while (x >= 100) {
            x -= 100;
        }
        if (x == 0) {
            ++b;
        }
        // cerr << i << " " << x << endl;
    }

    cout << "Result: " << x << " " << b << " " << c << " " << b + c << endl;
    return 0;
}
