#include <string>
#include <unordered_set>

#include "gflags/gflags.h"

#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/csv.h"
#include "lib/io/stream.h"
#include "lib/io/zstream.h"
#include "lib/progressBar.h"
#include "lib/timer.h"

DEFINE_string(in, "", "input");

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    Timer tTotal("Parse Reuters " + FLAGS_in);

    auto fIn = make_shared<FileInputStream>(FLAGS_in);
    auto fInProgress = make_shared<IFStreamProgressable>(fIn);
    ProgressBar::getInstance().setProgressable(fInProgress);
    auto zIn = make_shared<ZlibInputStream>(fIn);
    CsvParser reader(zIn);
    reader.readHeader();

    unordered_set<string> rics;

    const int iRic = reader.getIndex("#RIC");
    while (reader.readLine()) {
        auto ric = reader.get(iRic);
        if (!ric.empty()) {
            rics.insert(ric);
        }
    }

    for (const auto& ric : rics) {
        cout << ric << endl;
    }

    return 0;
}
