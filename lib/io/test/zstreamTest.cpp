#include "lib/io/zstream.h"
#include "lib/random.h"

#include "gtest/gtest.h"
#include "glog/logging.h"

TEST(ZInStream, Simple) {
    auto fIn = make_shared<ifstream>(
        "/home/denplusplus/Downloads/Kafka_Kafka-Franc-Romany_1_Propavshiy-bez-vesti.CpUnYQ.405255.fb2.gz");
    auto zIn = make_shared<ZIStream>(fIn);
    string line;
    int iLine = 0;
    while (getline(*zIn, line)) {
        cout << iLine << "\t" << line << endl;
        ++iLine;
    }
}

TEST(ZOutStream, Simple) {
    auto fOut = make_shared<ofstream>("test.gz");
    auto zOut = make_shared<ZOStream>(fOut);
    *zOut << "Test line 1" << endl;
    *zOut << "Test line 2" << endl;
}

TEST(ZOutStream, Large) {
    static const string FILENAME = "testLarge.gz";
    const string randS1 = randString(10000000);
    const string randS2 = randString(10000000);
    {
        auto fOut = make_shared<ofstream>(FILENAME);
        auto zOut = make_shared<ZOStream>(fOut);
        *zOut << randS1 << endl;
        *zOut << randS2 << endl;
    }
    {
        auto fIn = make_shared<ifstream>(FILENAME);
        auto zIn = make_shared<ZIStream>(fIn);
        string s;
        *zIn >> s;
        EXPECT_EQ(s.size(), randS1.size());
        EXPECT_EQ(s, randS1);
        s.clear();
        getline(*zIn, s);
        EXPECT_EQ(s.size(), randS2.size());
        EXPECT_EQ(s, randS2);
    }
}
