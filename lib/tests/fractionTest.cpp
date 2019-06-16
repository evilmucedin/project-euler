#include "lib/fraction.h"

#include "gtest/gtest.h"

TEST(Fraction, Simple) {
    Frac a(10, 20);
    Frac b(1, 3);
    EXPECT_EQ(a + b, Frac(5, 6));
}
