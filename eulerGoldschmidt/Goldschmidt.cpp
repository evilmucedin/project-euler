#include "lib/header.h"

#include "glog/logging.h"

int main(int argc, char* argv[]) {
	google::InitGoogleLogging(argv[0]);
	google::SetStderrLogging(google::INFO);
	LOG(INFO) << "Hello world";
	return 0;
}