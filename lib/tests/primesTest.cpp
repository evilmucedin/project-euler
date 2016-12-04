#include "gtest/gtest.h"

#include "lib/primes.h"

TEST(Totient, PrimesTest) {
    static constexpr int kN = 1000000;
    TotientErato terato(kN);
    Erato erato(kN);
    for (size_t i = 1; i < kN; ++i) {
        EXPECT_EQ(terato.totient_[i], eulerTotient(i, erato));
    }
}
