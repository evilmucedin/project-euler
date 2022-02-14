#include "lib/io/stream.h"

#include <gtest/gtest.h>

TEST(Stream, Stdout) {
    auto stream = new StdOutputStream();
    stream->write("test\n", 5);
}
