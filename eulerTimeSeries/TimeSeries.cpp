#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/timer.h"
#include "lib/io/zstream.h"
#include "lib/io/csv.h"

int main(int argc, char* argv[]) {
    Timer tTotal("Total");
    standardInit(argc, argv);
    auto fIn = make_shared<ifstream>("/home/denplusplus/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz", std::ifstream::binary);
    auto zIn = make_shared<ZIStream>(fIn);
    CsvParser reader(zIn);
    reader.readHeader();
    int iLine = 0;
    while (reader.readLine()) {
        if (0 == (iLine % 100000000)) {
            LOG(INFO) << OUT(iLine) << OUT(reader.size());
        }
        ++iLine;
    }
    LOG(INFO) << OUT(iLine);
    return 0;
}
