#include <lib/trie.h>

#include <gtest/gtest.h>

TEST(Trie, Simple) {
    Trie<char> t('a', 'z');
    t.addWord("russia");
    EXPECT_TRUE(t.hasWord<string>("russia"));
    EXPECT_TRUE(!t.hasWord<string>("usa"));
}
