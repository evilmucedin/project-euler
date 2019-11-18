#include "gtest/gtest.h"

#include "lib/atkinSieve.h"
#include "lib/primes.h"

TEST(AtkinSieveTest, Large) {
    auto test = [](size_t n) {
        Erato erato(n);
        AtkinSieve atkin(n);
        EXPECT_EQ(erato.primes_.size(), atkin.primes_.size());
        EXPECT_EQ(erato.primes_, atkin.primes_);
    };

    test(100);
    test(1000);
    test(1000000);
}
