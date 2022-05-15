#include "lib/header.h"
#include "lib/init.h"

#include "glog/logging.h"

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    i64 result = 0;

    LOG(OK) << OUT(result);
    LOG(INFO) << OUT(result);
    LOG(WARNING) << OUT(result);

    return 0;
}
