#include "gtest/gtest.h"

#include "lib/datetime.h"

TEST(Datetime, Simple) {
    DateTime dt("2017-11-28T01:00:00.070063844Z");
    EXPECT_EQ(dt.str(), "2017-11-28 01:00:00.070063843");
}

TEST(Datetime, Simple2) {
    DateTime dt("2017-11-28T01:00:00.99Z");
    EXPECT_EQ(dt.str(), "2017-11-28 01:00:00.989999999");
}
