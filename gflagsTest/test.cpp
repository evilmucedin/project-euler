    #include "gflags/gflags.h"
    #include <iostream>

    DEFINE_string(name, "World", "The name to greet.");

    int main(int argc, char* argv[]) {
        gflags::ParseCommandLineFlags(&argc, &argv, true);
        std::cout << "Hello, " << FLAGS_name << "!" << std::endl;
        return 0;
    }
