#include "lib/treap.h"

#include "gtest/gtest.h"

TEST(Treap, Simple) {
    Treap<int, int> t;
    EXPECT_EQ(t.size(), 0);
    t.emplace(1, 2);
    EXPECT_EQ(t.size(), 1);
    t.emplace(-1, 3);
    EXPECT_EQ(t.size(), 2);
    t.emplace(4, 4);
    EXPECT_EQ(t.size(), 3);
    EXPECT_EQ(t.get(1), 2);
    EXPECT_EQ(t.get(-1), 3);
    EXPECT_EQ(t.get(4), 4);
    EXPECT_EQ(t.size(), 3);
    t.erase(-1);
    t.erase(1);
    t.erase(4);
    EXPECT_EQ(t.size(), 0);
}
