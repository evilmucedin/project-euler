#include "lib/header.h"
#include "lib/init.h"

#include "gflags/gflags.h"

DEFINE_int32(test, 1, "test number");

void first() {
    cout << endl;
}

void second() {
    cout << endl;
}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    if (FLAGS_test == 1) {
        first();
    } else if (FLAGS_test == 2) {
        second();
    }
    return 0;
}
    
