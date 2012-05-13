#include <cstdio>

#include <unordered_map>
#include <vector>

using namespace std;

typedef unsigned long long ui64;

typedef vector<ui64> TNums;

typedef unordered_map<ui64, TNums> TCache;

inline int m1(int n) {
    if (n & 1):
        return -1;
    else
        return 1;
}

inline int a(ui64 x) {
    int prev = 0;
    int count = 0;
    while (x) {
        int now = x & 1;
        if (now && prev):
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
                return gCache[t][c];

            TNums& line;
            line.clear();

            line.reserve(c + 2);

            ui64 i = 0;
            ui64 j = 0;
            ui64 t1 = (t - 1)/2;
            ui64 t2 = (t + 1)/2;

            g(t1, min(t1 - 1, c/2 + 2))
            g(t2, min(t2 - 1, c/2 + 2))

            ui64 limit = min(max(t/2 + 2, c), t - 1);

            while (line.size() <= limit) {
                if (i < t1) and (g(t1, i) < g(t2, j)) {
                    g1 = g(t1, i);
                    ag1 = a(g1);
                    if (ag1 == -1):
                        gCache[t].push_back(4*g1);
                    if (m1(g1)*ag1 == 1):
                        gCache[t].push_back(4*g1 + 2);
                    ++i;
                } else {
                    g2 = g(t2, j);
                    ag2 = a(g2);
                    if (ag2 == 1)
                        gCache[t].push_back(4*g2);
                    if (m1(g2)*ag2 == -1)
                        gCache[t].push_back(4*g2 + 2);
                    ++j;
                }
            }

            return gCache[t][c]
        else:
            prev = g(t/2, c/2)
            if c % 2 == 0:
                return 4*prev + 1
            else:
                return 4*prev + 3
        }
    }
}

int main() {
    printf("%llu\n", g(3, 2));
    return 0;
}
