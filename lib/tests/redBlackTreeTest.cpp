#include "gtest/gtest.h"

#include "lib/redBlackTree.h"

TEST(RedBlackTree, Sum) {
    static constexpr int kSize = 12345;
    RedBlackTree<int> t;
    cout << t.searchTree(1) << endl;
    t.insert(2);
    cout << t.searchTree(2)->data << endl;
    cout << t.searchTree(1) << endl;
}
