#include <lib/trie.h>

#include <gtest/gtest.h>

TEST(Trie, Simple) {
    Trie<char> t(0, 'z');
    t.addWord(string("russia"));
    t.print();
    EXPECT_TRUE(t.hasWord(string("russia")));
    EXPECT_TRUE(!t.hasWord(string("usa")));
}
