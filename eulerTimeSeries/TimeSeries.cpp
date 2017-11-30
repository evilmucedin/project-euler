#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/timer.h"
#include "lib/io/zstream.h"

int main(int argc, char* argv[]) {
    Timer tTotal("Total");
    standardInit(argc, argv);
    auto fIn = make_shared<ifstream>("/home/denplusplus/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz", std::ifstream::binary);
    auto zIn = make_shared<ZIStream>(fIn);
    string line;
    int iLine = 0;
    while (getline(*zIn, line)) {
        if (0 == (iLine % 100000000)) {
            LOG(INFO) << OUT(iLine);
        }
        ++iLine;
    }
    LOG(INFO) << OUT(iLine);
    return 0;
}
