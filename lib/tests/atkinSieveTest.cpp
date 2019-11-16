#include "gtest/gtest.h"
#include "lib/atkinSieve.h"
#include "lib/primes.h"

TEST(AtkinSieveTest, Simple) {
    static constexpr size_t N = 1000000;
    Erato erato(N);
    AtkinSieve atkin(N);
    EXPECT_EQ(erato.primes_, atkin.primes_);
}
