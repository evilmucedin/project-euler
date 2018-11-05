#include "lib/io/keyvalue/blockStorage.h"

#include <gtest/gtest.h>

TEST(BlockStorage, Simple) {
    static const std::string FILENAME = "blockTest.dat";
    {
        BlockFileWriter writer(FILENAME);
        writer.add("1")->write("1234", 4);
        writer.add("zzzzzzzzzzzzzzzzzzzz")->write("lllllllllllllllllllllllll", 4);
    }
    {
        BlockFileReader reader(FILENAME);
        char buffer[5];
        buffer[4] = 0;
        reader.get("1")->read(buffer, 4);
        EXPECT_TRUE(strcmp(buffer, "1234") == 0);
        reader.get("zzzzzzzzzzzzzzzzzzzz")->read(buffer, 4);
        EXPECT_TRUE(strcmp(buffer, "llll") == 0);
    }
}
