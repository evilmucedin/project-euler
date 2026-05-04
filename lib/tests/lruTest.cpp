#include "lib/lru.h"

#include "gtest/gtest.h"

TEST(LRU, Simple) {
    LRUMap<int, int> data(10);
    data.emplace(1, 1);
    EXPECT_TRUE(data.find(1) != nullptr);
    EXPECT_EQ(*data.find(1), 1);
}

TEST(LRU, Capacity) {
    LRUMap<int, int> data(10);
    data.emplace(1, 1);
    EXPECT_TRUE(data.find(1) != nullptr);
    for (size_t i = 0; i < 10; ++i) {
        data.emplace(i + 20, i + 20);
        EXPECT_TRUE(data.find(i + 20) != nullptr);
    }
    EXPECT_TRUE(data.find(1) == nullptr);
    for (size_t i = 0; i < 10; ++i) {
        EXPECT_TRUE(data.find(i + 20) != nullptr);
    }
    EXPECT_EQ(data.size(), 10);
    EXPECT_TRUE(data.find(1) == nullptr);
    EXPECT_TRUE(data.find(20) != nullptr);
    EXPECT_TRUE(data.find(29) != nullptr);
}
