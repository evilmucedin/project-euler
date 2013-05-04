#include <cstdio>

int main() {
    typedef long long int TLLI;
    static const TLLI N = 2*3*5*7*11*13*17;

    for (TLLI i = 2; i < N; ++i) {
        if ( ((i*i*i) % N) == 1 )
            printf("%d\n", (int)i);
    }

    return 0;
}
