#include <cstdio>
#include <cmath>

typedef unsigned long long int ui64;

int main() {
    // static const ui64 N = 10000;
    static const ui64 N = 10000000000000000ULL;
    static const double TAU2 = (3. + sqrt(5.))/2;

    // static const ui64 MOD = 282475249;
    static const ui64 MOD = 1ULL << 63;

    ui64 result = 0;
    for (ui64 i = 1; i <= N; ++i) {
        if (i % 1000000 == 0)
            printf("%lld %.10lf\n", i, 100.*double(i)/N);
        ui64 end = i - 1;
        ui64 begin = i - double(i)/TAU2 + 1;
        // printf("%lld %lld %lld\n", i, begin, end);

        if (begin <= end) {
            ui64 sum = begin + end;
            ui64 diff = end - begin + 1;

            bool needDiv2 = true;
            if ((sum & 1) == 0) {
                sum /= 2;
                needDiv2 = false;
            }
            if (needDiv2)
                diff /= 2;
            sum %= MOD;
            diff %= MOD;

            result += sum*diff;
            result %= MOD;

            diff = (end - begin + 1) % MOD;
            result += diff*(i % MOD);
            result %= MOD;
        }
    }
    printf("%lld\n", result);

    return 0;
}
