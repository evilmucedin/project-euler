#include "lib/header.h"
#include "lib/init.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

int main(int argc, char* argv[]) {
    standardInit(argc, argv);
    return 0;
}
