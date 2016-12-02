#include <cstdlib>

#include "gtest/gtest.h"

#include "lib/radixSort.h"
#include "lib/timer.h"

TEST(RadixSortRand, RadixSortTest) {
    static constexpr size_t n = 1 << 24;
    U32Vector vct(n);
    for (size_t i = 0; i < n; ++i) {
        vct[i] = rand();
    }
    U32Vector copy1(vct);
    U32Vector copy2(vct);
    {
        Timer t("radix");
        radixSort(copy1);
    }
    {
        Timer t("std::sort");
        sort(vct.begin(), vct.end());
    }
    for (size_t i = 1; i < n; ++i) {
        EXPECT_LE(vct[i - 1], vct[i]);
    }
}
