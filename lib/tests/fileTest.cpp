#include "glog/logging.h"
#include "gtest/gtest.h"
#include "lib/file.h"

TEST(File, BufStringReader) {
    google::InitGoogleLogging("test");
    cerr << "File::BufferedStringReader" << endl;
    LOG(INFO) << "File::BufferedStringReader";
    const auto ws = stringToWString("{{infobox }}");
    BufferedStringReader br(ws);
    LOG(INFO) << OUT(br.eof()) << OUT(br.offset()) << OUT(static_cast<unsigned char>(br.peek()));
    size_t count = 0;
    while (!br.eof()) {
        fprintf(stderr, "ad=%d\n", static_cast<int>(br.advance()));
        ++count;
    }
    fflush(stderr);
    LOG(INFO) << OUT(count);
}

TEST(File, Utf8) {
    setlocale(LC_ALL, "en_US.UTF8");

    File f("lib/tests/utf8test.txt", "r");
    size_t count = 0;
    while (!f.eof()) {
        WChar ch = f.getUTF8C();
        if (ch != WEOF) {
            fwprintf(stderr, L"%lc", ch);
        }
        ++count;
    }
    fwprintf(stderr, L"\n%zd\n", count);
}
