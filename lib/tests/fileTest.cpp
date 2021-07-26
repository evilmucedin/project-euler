#include "gtest/gtest.h"

#include "lib/file.h"

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
