#include "lib/exception.h"

#include "gtest/gtest.h"

TEST(Exception, Stream) {
    StreamException e("Test ");
    e << 1;
    throw e;
}

TEST(Exception, Stream2) {
    THROW("Test " << 1);
}
