#include "gtest/gtest.h"
#include "lib/io/zstream.h"

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
    *zOut << "Test line" << endl;
}
