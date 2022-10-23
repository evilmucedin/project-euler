#include "lib/header.h"
#include "lib/init.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(input, "", "input file");

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    return 0;
}
