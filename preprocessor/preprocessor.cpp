#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/stream.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(input, "-", "input file");
DEFINE_string(output, "-", "output file");

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    PInputStream fIn = openInputStream(FLAGS_input);
    POutputStream fOut = openOutputStream(FLAGS_output);
    string line;
    while (fIn->readLine(line)) {
        fOut->writeLine(line);
    }

    return 0;
}
