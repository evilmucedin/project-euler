#include "lib/exception.h"

#include "gtest/gtest.h"

TEST(Exception, Stream) {
    EXPECT_THROW(
        {
            StreamException e("Test ");
            e << 1;
            throw e;
        },
        StreamException);
}

TEST(Exception, Stream2) {
    EXPECT_THROW({ THROW("Test " << 1); }, StreamException);
}
