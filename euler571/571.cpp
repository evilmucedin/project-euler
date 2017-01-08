#include "lib/header.h"

bool isPandigital(i64 num, int base) {
    BoolVector mask(base);
    while (num) {
        mask[num % base] = true;
        num /= base;
    }
    for (auto flag: mask) {
        if (!flag) {
            return false;
        }
    }
    return true;
}

int main() {
    static constexpr int N = 12;
    IntVector v(N);
    for (int i = 0; i < N; ++i) {
        v[i] = i;
    }
    swap(v[0], v[1]);
    i64 sum = 0;
    int index = 0;
    do {
        i64 num = 0;
        for (auto n: v) {
            num = N*num + n;
        }

        int base = N - 1;
        while (base > 1 && isPandigital(num, base)) {
            --base;
        }
        if (base == 1) {
            sum += num;
            ++index;
            cout << index << "\t" << num << "\t" << sum << endl;
        }

    } while (next_permutation(v.begin(), v.end()));

    return 0;
}
