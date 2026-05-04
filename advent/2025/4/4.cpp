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
    auto lines = readInputLines();

    const int x = lines.size();
    const int y = lines[0].size();

    int sum = 0;

    for (int i = 0; i < x; ++i)
        for (int j = 0; j < y; ++j) {
            if (lines[i][j] == '@') {
                int z = 0;
                for (int ii = -1; ii <= 1; ++ii)
                    for (int jj = -1; jj <= 1; ++jj) {
                        int iii = i + ii;
                        int jjj = j + jj;
                        if (iii >= 0 && iii < x)
                            if (jjj >= 0 && jjj < y)  {
                                if (lines[iii][jjj] == '@')
                                    ++z;
                            }
                    }
                if (z < 5)
                    ++sum;
            }
        }

    cout << sum << endl;

    return 0;

}
