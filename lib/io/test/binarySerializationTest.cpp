#include "gtest/gtest.h"

#include "lib/header.h"
#include "lib/io/stream.h"
#include "lib/io/binarySerialization.h"

TEST(BinarySerialization, Simple) {
    static const string FILENAME = "serializationTest.bin";
    {
        auto s = openFileBufferedWriter(FILENAME);
        int a = 4;
        binarySerialize(*s, a);
    }
    {
        auto s = openFileBufferedReader(FILENAME);
        int a = binaryDeserialize<int>(*s);
        EXPECT_EQ(a, 4);
    }
}

TEST(BinarySerialization, String) {
    static const string FILENAME = "serializationSringTest.bin";
    static const string S = "Hello World!";
    {
        auto s = openFileBufferedWriter(FILENAME);
        string str = S;
        binarySerialize(*s, str);
    }
    {
        auto s = openFileBufferedReader(FILENAME);
        const auto str = binaryDeserialize<string>(*s);
        EXPECT_EQ(str, S);
    }
}
