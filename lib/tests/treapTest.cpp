#include "lib/treap.h"

#include "gtest/gtest.h"

TEST(Treap, Simple) {
    Treap<int, int> t;
    t.emplace(1, 2);
}
