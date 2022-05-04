#include "gtest/gtest.h"

#include "lib/header.h"
#include "lib/random.h"
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
    static const string FILENAME = "serializationStringTest.bin";
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

TEST(BinarySerialization, Vector) {
    static const string FILENAME = "serializationVectorTest.bin";
    const size_t n = dice(1000);
    vector<string> v(n);
    for (auto& s: v) {
        s = randString(10);
    }
    {
        auto s = openFileBufferedWriter(FILENAME);
        binarySerialize(*s, v);
    }
    {
        auto s = openFileBufferedReader(FILENAME);
        const auto v2 = binaryDeserialize<vector<string>>(*s);
        EXPECT_EQ(v, v2);
    }
}
