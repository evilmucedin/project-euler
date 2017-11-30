#include "lib/header.h"
#include "glog/logging.h"

void standardInit(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    google::InitGoogleLogging(argv[0]);
}
