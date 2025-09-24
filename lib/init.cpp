#include <log4cplus/initializer.h>

#include "glog2/src/glog/logging.h"
#include "gflags/gflags.h"

#include "lib/header.h"

#include "lib/init.h"

/*
LoggingInitializer::LoggingInitializer() {
  std::cout << "Logging system initialized." << std::endl;
}

LoggingInitializer& LoggingInitializer::get() {
  static LoggingInitializer instance;
  return instance;
}

*/

void standardInit(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    LoggingInitializer::get();
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    google::InitGoogleLogging(argv[0]);
}
