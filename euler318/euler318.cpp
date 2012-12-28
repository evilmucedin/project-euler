#include <cstdio>
#include <cmath>

int main() {
    int sum = 0;
    for (int p = 1; p <= 1005; ++p) {
        for (int q = p + 1; p + q <= 2011; ++q) {
            double b0 = sqrt((double)q) - sqrt((double)p);
            if (b0 >= 1.0)
                break;
            sum += (int)ceil(-2011.0/2./log10(b0));
        }
    }
    printf("%d\n", sum);
    return 0;
}
