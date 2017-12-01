#include "lib/string.h"

#include <cctype>

#include "gtest/gtest.h"

TEST(Split, Empty) { EXPECT_TRUE(split("") == (StringVector){}); }

TEST(Split, One) { EXPECT_TRUE(split("a") == (StringVector){"a"}); }

TEST(Split, Two) { EXPECT_TRUE(split("a,b", ',') == ((StringVector){"a", "b"})); }

TEST(Split, Three) { EXPECT_TRUE(split("a,,b", ',') == ((StringVector){"a", "", "b"})); }

TEST(Split, Three2) { EXPECT_TRUE(split(",a,b", ',') == ((StringVector){"", "a", "b"})); }

TEST(Functor, Simple) {
    auto splitResult = splitByFunctor("0aaa111bb3", [](char ch) { return isdigit(ch); });
    cerr << splitResult << endl;
    EXPECT_TRUE(splitResult == ((StringVector){"aaa", "bb"}));
}
