#include "lib/header.h"
#include "glog/logging.h"
#include "gflags/gflags.h"

void standardInit(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    LoggingInitializer::get();
    gflags::ParseCommandLineFlags(&argc, &argv, true);
}
