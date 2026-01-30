#include "lib/header.h"

#include "gtest/gtest.h"

TEST(Header, numToString) {
    auto s = numToString<int>(123);
    EXPECT_EQ(s, "123");
    s = numToString<int>(-123);
    EXPECT_EQ(s, "-123");
}
