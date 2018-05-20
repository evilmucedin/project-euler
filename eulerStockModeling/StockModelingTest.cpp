#include "gtest/gtest.h"

#include "orderBook.h"

TEST(OrderBook, Simple) {
    OrderBook b;
    b.addOrder(Order(true, 10, 1));
    b.addOrder(Order(false, 10, 1));
    auto res = b.cross();
    EXPECT_EQ(res.size(), 1);
}
