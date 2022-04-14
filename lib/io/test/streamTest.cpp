#include "lib/io/stream.h"
#include "lib/random.h"
#include "lib/header.h"
#include "lib/timer.h"

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
    static const string FILENAME = "streamTest1.bin";
    auto sFOut = make_shared<FileOutputStream>(FILENAME);
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
    sBOut.reset();
    sFOut.reset();
    auto sFIn = make_shared<FileInputStream>(FILENAME);
    auto sBIn = make_shared<BufferedInputStream>(sFIn, 1 << 11);
    size_t read = 0;
    size_t readTotal = 0;
    while ((read = sBIn->read(buffer, 1024))) {
        readTotal += read;
    }
    EXPECT_EQ(readTotal, slen);
}

template <typename T>
void benchmark(T& s) {
    Timer t("benchmark");
    for (size_t i = 0; i < 10000000; ++i) {
        s << rand();
    }
}

template <typename T>
void benchmark2(T& s) {
    Timer t("benchmark2");
    for (size_t i = 0; i < 10000000; ++i) {
        s.printf("%d", rand());
    }
}

TEST(Stream, Benchmark) {
    ofstream of("test1.bin");
    benchmark(of);
    auto sFOut = make_shared<FileOutputStream>("test2.bin");
    auto sBOut = make_shared<BufferedOutputStream>(sFOut, 1 << 16);
    benchmark(*sBOut);
    File fOut("test3.bin", "wb");
    fOut.setvbuf(1 << 16);
    benchmark2(fOut);
}
