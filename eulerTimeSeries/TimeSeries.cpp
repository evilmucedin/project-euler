#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/fstream.h"
#include "lib/io/zstream.h"
#include "lib/timer.h"

int main(int argc, char* argv[]) {
  Timer tTotal("Total");
  standardInit(argc, argv);
  auto fIn = make_shared<IFstream>(
      homeDir() + "/Downloads/NSQ-2017-11-28-MARKETPRICE-Data-1-of-1.csv.gz",
      std::ifstream::binary);
  auto zIn = make_shared<ZIStream>(fIn);
  CsvParser reader(zIn);
  reader.readHeader();
  int iLine = 0;
  int iFIDNumber = reader.getIndex("Number of FIDs");
  int iType = reader.getIndex("UpdateType/Action");
  while (reader.readLine()) {
      int nFids = reader.getInt(iFIDNumber);
      LOG_EVERY_MS(INFO, 1000) << OUT(reader.get(iType)) << OUT(nFids);
      for (int iFid = 0; iFid < nFids; ++iFid) {
        reader.readLine();
      }
  }
  LOG(INFO) << OUT(iLine);
  return 0;
}
