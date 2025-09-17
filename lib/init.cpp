#include "glog2/src/glog/logging.h"
#include "gflags/gflags.h"

#include "lib/header.h"

void standardInit(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    // LoggingInitializer::get();
    gflags::ParseCommandLineFlags(&argc, &argv, true);
}
