#include <cstdio>
#include <cmath>
#include <vector>

using namespace std;

typedef unsigned long long int ui64;

typedef vector<ui64> TNumbers;

static ui64 Count(ui64 a) {
    if (a & 1)
        return ((a + 3)*(a + 3) + 23)/48;
    else
        return (a*a + 23)/48;
}

static ui64 StCount(ui64 a) {
    ui64 count = 0;
    for (ui64 i = 1; i <= a; ++i)
        for (ui64 j = i; j <= a - i; ++j) {
            ui64 k = a - i - j;
            if (k >= j && i + j > k && i + k > j && j + k > i)
                ++count;
        }
    return count;
}

static int GCD(int a, int b) {
    while (b) {
        int temp = a % b;
        a = b;
        b = temp;
    }
    return a;
}

static int GCD(int a, int b, int c) {
    return GCD(GCD(a, b), c);
}

int main() {
    static const int N = 10000000;
    // static const int N = 100;

    TNumbers triangles(N + 1);
    for (int i = 1; i <= N; ++i) {
        triangles[i] = Count(i);
        // printf("%d %lld %lld\n", i, Count(i), StCount(i));
    }

    /*
    TNumbers triangles2(N + 1);

    for (ui64 i = 1; i <= N; ++i) {
        for (ui64 j = i; j <= N - i; ++j) {
            for (ui64 k = j; k <= N - i - j; ++k) {
                if (i + j > k && i + k > j && j + k > i)
                    if (GCD(i, j, k) == 1)
                        ++triangles2[i + j + k];
            }
        }
    }
    */

    for (int i = 3; i <= N; ++i) {
        // printf("%lld %lld\n", triangles[i], triangles2[i]);
        for (ui64 j = 2*i; j <= N; j += i) {
            triangles[j] -= triangles[i];
        }
    }

    ui64 sum = 0;
    for (ui64 i = 3; i <= N; ++i)
        sum += triangles[i];

    printf("%lld\n", sum);

    return 0;
}
