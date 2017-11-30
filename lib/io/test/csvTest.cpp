#include "gtest/gtest.h"

#include "lib/io/csv.h"

TEST(CsvReaderTest, Simple) {
    static const string kCsv =
R"csv(A,B,C
1,test,"str""ing")csv";
    auto stream = make_shared<istringstream>(kCsv);
    CsvParser reader(stream);
    EXPECT_TRUE(reader.readHeader());
    EXPECT_EQ(reader.size(), 3);
    EXPECT_EQ(reader.get(0), "A");
    EXPECT_EQ(reader.get(1), "B");
    EXPECT_EQ(reader.get(2), "C");
    EXPECT_TRUE(reader.readLine());
    EXPECT_EQ(reader.size(), 3);
    EXPECT_EQ(reader.get(0), "1");
    EXPECT_EQ(reader.get(1), "test");
    EXPECT_EQ(reader.get(2), "str\"ing");
    EXPECT_FALSE(reader.readLine());
}
