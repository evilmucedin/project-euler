#include "lib/io/stream.h"
#include "lib/random.h"
#include "lib/header.h"

#include <gtest/gtest.h>
#include <glog/logging.h>

TEST(Stream, Stdout) {
    auto stream = new StdOutputStream();
    stream->write("test\n", 5);
}

TEST(Stream, File) {
    auto sFOut = make_shared<FileOutputStream>("streamTest.bin");
    auto sBOut = make_shared<BufferedOutputStream>(sFOut, 1 << 11);
    char buffer[1024];
    size_t slen = 0;
    for (size_t i = 0; i < 1000; ++i) {
        const size_t len = randAB(1, 1000);
        for (size_t j = 0; j < len; ++j) {
            buffer[j] = randAB<char>('a', 'z');
        }
        sBOut->write(buffer, len);
        slen += len;
    }
    LOG(INFO) << OUT(slen);
}

TEST(Stream, File1) {
    auto sFOut = make_shared<FileOutputStream>("streamTest1.bin");
    auto sBOut = make_shared<BufferedOutputStream>(sFOut, 1 << 11);
    char buffer[1024];
    size_t slen = 0;
    for (size_t i = 0; i < 1000; ++i) {
        const size_t len = randAB(1, 1000);
        for (size_t j = 0; j < len; ++j) {
            buffer[j] = '1';
        }
        sBOut->write(buffer, len);
        slen += len;
    }
    LOG(INFO) << OUT(slen);
}
