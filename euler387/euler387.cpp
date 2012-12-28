#include <cstdio>
#include <cmath>

typedef __int64 i64;

i64 sum = 0;

static bool IsPrime(i64 number) {
    if (number < 2)
        return false;
    if (number == 2)
        return true;
    if (number & 1) {
        i64 top = (i64)sqrt((double)number);
        for (size_t i = 3; i <= top; i += 2)
            if (0 == (number % i))
                return false;
        return true;
    } else {
        return false;
    }
}

static void Enum(i64 number, i64 ds, int digits) {
    if (digits < 14) {
        if (0 == (number % ds)) {
            for (i64 i = 0; i < 10; ++i) {
                const i64 cand = 10*number + i;
                if (IsPrime(number/ds) && IsPrime(cand))
                    sum += cand;
                Enum(cand, ds + i, digits + 1);
            }
        }
    }
}

int main() {
    for (int i = 1; i < 10; ++i)
        Enum(i, i, 1);
    printf("%I64d\n", sum);
    return 0;
}
