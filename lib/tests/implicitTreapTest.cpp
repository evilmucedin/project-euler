#include "lib/implicitTreap.h"

#include "gtest/gtest.h"

TEST(ImplicitTreap, Simple) {
    ImplicitTreap<int> t;
    EXPECT_EQ(t.size(), 0);
    t.push_back(1);
    EXPECT_EQ(t.at(0), 1);
    t.push_back(2);
    EXPECT_EQ(t.at(0), 1);
    EXPECT_EQ(t.at(1), 2);
    EXPECT_EQ(t.size(), 2);
}
