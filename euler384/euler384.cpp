#include <cstdio>

#include <unordered_map>
#include <vector>

using namespace std;
using namespace std;

typedef unsigned long long ui64;

typedef vector<ui64> TNums;

typedef unordered_map<ui64, TNums> TCache;

inline int m1(int n) {
    if (n & 1)
        return -1;
    else
        return 1;
}

inline int a(ui64 x) {
    int prev = 0;
    int count = 0;
    while (x) {
        int now = x & 1;
        if (now && prev)
            ++count;
        prev = now;
        x /= 2;
    }
    return m1(count);
}

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

            ui64 limit = min(max(t*2/3 + 10, c), t - 1);
            line.reserve(limit + 1);

            ui64 i = 0;
            ui64 j = 0;
            ui64 t1 = (t - 1)/2;
            ui64 t2 = (t + 1)/2;

            // g(t1, min(t1 - 1, c/2 + 2));
            // g(t2, min(t2 - 1, c/2 + 2));
            printf("%llu %llu\n", t, c);
            while (line.size() <= limit) {
                if ((i < t1) && (g(t1, i) < g(t2, j))) {
                    ui64 g1 = g(t1, i);
                    ui64 ag1 = a(g1);
                    if (ag1 == -1)
                        line.push_back(4*g1);
                    if (m1(g1)*ag1 == 1)
                        line.push_back(4*g1 + 2);
                    ++i;
                } else {
                    ui64 g2 = g(t2, j);
                    ui64 ag2 = a(g2);
                    if (ag2 == 1)
                        line.push_back(4*g2);
                    if (m1(g2)*ag2 == -1)
                        line.push_back(4*g2 + 2);
                    ++j;
                }
            }

            return line[c];
        } else {
            ui64 prev = g(t/2, c/2);
            if (c % 2 == 0)
                return 4*prev + 1;
            else
                return 4*prev + 3;
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
        printf("%llu %llu %llu %llu %llu\n", i + 2, fa, fb, res, sum);
        sum += res;
    }
    printf("%llu\n", sum);

    return 0;
}
