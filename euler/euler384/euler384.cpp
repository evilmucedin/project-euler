#include <cstdio>

#include <unordered_map>
#include <vector>

using namespace std;
using namespace std;

typedef unsigned long long ui64;

typedef vector<ui64> TNums;

typedef unordered_map<ui64, TNums> TCache;

TCache cache;

ui64 g(ui64 t, ui64 c) {
    if (t == 1 && c == 0) {
        return 0;
    } else {
        if (t & 1) {
            if (c < cache[t].size())
                return cache[t][c];

            TNums& line = cache[t];
            line.clear();

            const ui64 limit = min(max(t*2/3 + 5, c), t - 1);
            line.reserve(limit + 1);

            ui64 i = 0;
            ui64 j = 0;
            const ui64 t1 = (t - 1)/2;
            const ui64 t2 = (t + 1)/2;

            // g(t1, min(t1 - 1, c/2 + 2));
            // g(t2, min(t2 - 1, c/2 + 2));
            printf("%llu %llu\n", t, c);
            while (line.size() <= limit) {
                ui64 cg = g(t2, j);
                int sign = 0;
                if (i < t1) {
                    ui64 g1 = g(t1, i);
                    if (g1 < cg) {
                        cg = g1;
                        sign = 1;
                        ++i;
                    } else {
                        ++j;
                    }
                } else {
                    ++j;
                }

                int count2 = 0;
                {
                    ui64 x = cg;
                    int prev = 0;
                    while (x) {
                        int now = x & 1;
                        count2 += (now && prev);
                        prev = now;
                        x >>= 1;
                    }
                }

                if ( (count2 & 1) == sign )
                    line.push_back(4*cg);
                if (((cg & 1) ^ (count2 & 1)) == !sign)
                    line.push_back(4*cg + 2);
            }
            // printf("\n");

            return line[c];
        } else {
            const ui64 prev = g(t >> 1, c >> 1);
            if (c & 1)
                return 4*prev + 3;
            else
                return 4*prev + 1;
        }
    }
}

int main() {
    ui64 fa = 1;
    ui64 fb = 1;
    ui64 sum = 0;
    for (ui64 i = 0; i < 44; ++i) {
        ui64 temp = fa;
        fa = fa + fb;
        fb = temp;
        ui64 res = g(fa, fb - 1);
        sum += res;
        printf("%llu %llu %llu %llu %llu\n", i + 2, fa, fb, res, sum);
        cache.clear();
    }
    printf("%llu\n", sum);

    return 0;
}
