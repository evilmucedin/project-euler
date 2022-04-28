#include "lib/io/stream.h"
#include "lib/io/zstream.h"
#include "lib/random.h"
#include "lib/io/utils.h"

#include "gtest/gtest.h"
#include "glog/logging.h"

TEST(ZInStream, Simple) {
    auto fIn = make_shared<FileInputStream>(repoRoot() + "/lib/io/test/Kafka_Kafka-Franc-Romany_3_Zamok.Sf2l2w.349158.fb2.gz");
    auto zIn = make_shared<ZlibInputStream>(fIn);
    string line;
    int iLine = 0;
    while (zIn->readLine(line)) {
        // cout << iLine << "\t" << line << endl;
        ++iLine;
        if (iLine < 10) {
            cout << line << endl;
        }
    }
}

TEST(ZOutStream, Simple) {
    {
        auto fOut = make_shared<FileOutputStream>("test.gz");
        auto zOut = make_shared<ZlibOutputStream>(fOut);
        *zOut << "Test line 1" << Endl;
        *zOut << "Test line 2" << Endl;
    }
    {
        auto fIn = make_shared<FileInputStream>("test.gz");
        auto zIn = make_shared<ZlibInputStream>(fIn);
        cerr << zIn->readLine() << endl;
        cerr << zIn->readLine() << endl;
    }
}

/*
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
*/
