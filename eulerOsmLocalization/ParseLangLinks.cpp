#include <cstdio>

#include "lib/header.h"
#include "lib/string.h"
#include "glog/logging.h"

static constexpr char FILENAME[] = "eulerOsmLocalization/enwiki-20210601-langlinks.sql";

int main() {
    FILE* fIn = fopen(FILENAME, "rb");
    char ch;
    size_t count = 0;
    while (EOF != (ch = fgetc(fIn))) {
        ++count;
        if (0 == count % 10000) {
            LOG_EVERY_MS(INFO, 1000) << OUT(bytesToStr(count));
        }
    }

    LOG(INFO) << OUT(count);

    return 0;
}
