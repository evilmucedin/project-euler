#include "glog/logging.h"
#include "lib/datetime.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"
#include "lib/timer.h"

int main(int argc, char* argv[]) {
    Timer tTotal("Total");
    standardInit(argc, argv);
    auto fIn = make_shared<IFStream>(homeDir() + "/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz", std::ifstream::binary);
    auto zIn = make_shared<ZIStream>(fIn);
    CsvParser reader(zIn);
    reader.readHeader();
    int iLine = 0;
    const int iFIDNumber = reader.getIndex("Number of FIDs");
    const int iType = reader.getIndex("UpdateType/Action");
    const int iDateTime = reader.getIndex("Date-Time");
    const int iFidName = reader.getIndex("FID Name");
    const int iFidValue = reader.getIndex("FID Value");
    const int iRic = reader.getIndex("#RIC");
    OFStream fOut("aapl.tsv");
    while (reader.readLine()) {
        int nFids = reader.getInt(iFIDNumber);
        auto recordType = reader.get(iType);
        if (recordType == "TRADE") {
            auto ric = reader.get(iRic);
            DateTime dateTime(reader.get(iDateTime));
            double price = 0;
            double volume = 0;
            for (int iFid = 0; iFid < nFids; ++iFid) {
                reader.readLine();
                auto fidName = reader.get(iFidName);
                if (fidName == "TRDPRC_1") {
                    price = reader.getDouble(iFidValue);
                } else if (fidName == "TRDVOL_1") {
                    volume = reader.getDouble(iFidValue);
                }
            }
            if (ric == "AAPL.O") {
                fOut << dateTime.time_.time_ << "\r" << price << "\t" << volume << endl;
            }
            LOG_EVERY_MS(INFO, 1000) << OUT(ric) << OUT(dateTime.str()) << OUT(price) << OUT(volume);
        } else {
            reader.skipLines(nFids);
        }
    }
    LOG(INFO) << OUT(iLine);
    return 0;
}
