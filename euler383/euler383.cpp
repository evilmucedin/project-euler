#include <cstdio>

typedef unsigned long long ui64;
typedef unsigned char ui8;
typedef char i8;

ui64 n;

static const int LEN = 26;
static const int POWER = 18;

int mod5[LEN];
int now[LEN];

static const bool VERBOSE = POWER < 5;

ui64 Generate(ui8 index, bool small, ui8 defect) {
    if ( (index < 6) || VERBOSE) {
        for (size_t i = 0; i < index; ++i)
            printf("%d", now[i]);
        printf(" %d\n", (int)defect);
        fflush(stdout);
    }

    if (index == LEN) {
        i8 count = 0;
        while ( (count < LEN) && (now[LEN - 1 - count] == 0) )
            ++count;
        if (defect < count) {
            if (VERBOSE) {
                printf("!");
                for (size_t i = 0; i < index; ++i)
                    printf("%d", now[i]);
                printf("\n");
            }
            return 1;
        } else {
            return 0;
        }
    }

    ui8 limit = (small) ? 4 : mod5[index];
    if (index + defect + 1 >= LEN)
        limit = 0;
    ui64 sum = 0;
    for (ui8 next = 0; next <= limit; ++next) {
        now[index] = next;
        ui8 d2 = 0;
        if (next > 2) {
            while ( index >= d2 + 1 && now[index - d2 - 1] == 2 )
                ++d2;
            ++d2;
        }
        ui64 g = Generate(index + 1, small || (next < mod5[index]), defect + d2);
        if (small) {
            if ((0 == next || 3 == next) && (next + 1 <= limit)) {
                g += g;
                ++next;
            }
        }
        sum += g;
    }
    return sum;
}

int main() {
    n = 1;
    for (size_t i = 0; i < POWER; ++i)
        n *= 10;
    printf("%lld\n", n);
    for (int i = LEN - 1; i >= 0; --i) {
        mod5[i] = n % 5;
        n /= 5;
    }
    for (size_t i = 0; i < LEN; ++i)
        printf("%d", mod5[i]);
    printf("\n");

    printf("%lld\n", Generate(0, false, 0) - 1);

    return 0;
}
