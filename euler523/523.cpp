#include "lib/header.h"

int main() {
    static constexpr size_t kM = 100000;
    double ops = 0.0;

    static constexpr size_t kN = 30;
    I32Vector v(kN);
    for (size_t i = 0; i < kN; ++i) {
        v[i] = i;
    }

    for (size_t i = 0; i < kM; ++i) {
        random_shuffle(v.begin(), v.end());
    }

    return 0;
}
