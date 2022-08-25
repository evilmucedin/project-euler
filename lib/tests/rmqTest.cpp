#include "lib/random.h"
#include "lib/rmq.h"

#include "gtest/gtest.h"

TEST(RMQ, LogCeil) {
    EXPECT_EQ(1, logCeil<u32>(1));
    EXPECT_EQ(1, logCeil<u64>(1));
    EXPECT_EQ(2, logCeil<u32>(2));
    EXPECT_EQ(2, logCeil<u64>(2));
    EXPECT_EQ(2, logCeil<u32>(3));
    EXPECT_EQ(2, logCeil<u64>(3));
    EXPECT_EQ(3, logCeil<u32>(4));
    EXPECT_EQ(3, logCeil<u64>(4));
    EXPECT_EQ(3, logCeil<u32>(5));
    EXPECT_EQ(3, logCeil<u64>(5));
    EXPECT_EQ(11, logCeil<u32>(1024));
    EXPECT_EQ(11, logCeil<u64>(1024));
}

TEST(RMQ, Test1) {
    static constexpr size_t kN = 10000;
    using SizeTVector = std::vector<size_t>;
    SizeTVector v(kN);
    for (size_t i = 0; i < kN; ++i) {
        v[i] = i;
    }
    RangeAggQuery<SizeTVector::const_iterator, MinF<size_t>> rmq(v.begin(), v.end());
    EXPECT_EQ(*rmq.agg(v.begin(), v.end()), 0);
    EXPECT_EQ(*rmq.agg(v.begin(), v.begin() + 1), 0);
}

TEST(RMQ, TestRandom) {
    static constexpr size_t kN = 10000;
    using SizeTVector = std::vector<size_t>;
    SizeTVector v(kN);
    for (size_t i = 0; i < kN; ++i) {
        v[i] = randAB(0, 100000);
    }
    RangeAggQuery<SizeTVector::const_iterator, MinF<size_t>> rmq(v.begin(), v.end());
    for (size_t i = 0; i < 1000; ++i) {
        auto begin = dice(v.size());
        auto end = dice(v.size());
        if (begin > end) {
            std::swap(begin, end);
        }
        EXPECT_EQ(*rmq.agg(v.begin() + begin, v.begin() + end), *std::min_element(v.begin() + begin, v.begin() + end));
    }
}
