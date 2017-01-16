#include "gtest/gtest.h"

#include "lib/primes.h"

TEST(PrimesTest, Totient) {
    static constexpr int kN = 1000000;
    TotientErato terato(kN);
    Erato erato(kN);
    for (size_t i = 1; i < kN; ++i) {
        EXPECT_EQ(terato.totient_[i], eulerTotient(i, erato));
    }
}


TEST(PrimesTest, Pi) {
    static constexpr int kN = 10000;
    Erato erato(kN);
    size_t pi = 0;
    for (size_t i = 1; i < kN; ++i) {
        if (erato.isPrime(i)) {
            ++pi;
        }
        EXPECT_EQ(pi, erato.pi(i));
    }
}

TEST(PrimesTest, Pi3) {
    static constexpr int kN = 100000;
    Erato erato(kN);
    EXPECT_EQ(pi(100000000, erato), 5761455);
}

TEST(PrimesTest, Pi2) {
    static constexpr int kN = 100000;
    Erato erato(kN/10);
    Erato erato2(kN);
    for (size_t i = 1; i < kN; ++i) {
        EXPECT_EQ(pi(i, erato), erato2.pi(i));
    }
}

TEST(PrimesTest, CountDivisors) {
    static constexpr int kN = 100000;
    FactorizationErato ferato(kN);
    Erato erato(kN);
    for (size_t i = 1; i < kN; ++i) {
        EXPECT_EQ(ferato.factorize(i), factorization(i, erato));
    }
}
