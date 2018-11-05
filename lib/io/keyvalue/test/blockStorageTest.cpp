#include "lib/io/keyvalue/blockStorage.h"

#include <gtest/gtest.h>

TEST(BlockStorage, Simple) {
    static const std::string FILENAME = "blockTest.dat";
    {
        BlockFileWriter writer(FILENAME);
    }
    {
        BlockFileReader reader(FILENAME);
    }
}
