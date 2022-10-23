#include "lib/header.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/io/stream.h"
#include "lib/io/utils.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(input, "-", "input file");
DEFINE_string(output, "-", "output file");

string normalizeLine(const string& line) {
    return trim(line);
}

bool processInclude(const string& line, const string& root, POutputStream fOut) {
    string inlude;
    if (include.empty()) {
        return false;
    }

}

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    string root = repoRoot();
    LOG(INFO) << OUT(root);

    PInputStream fIn = openInputStream(FLAGS_input);
    POutputStream fOut = openOutputStream(FLAGS_output);
    string line;
    while (fIn->readLine(line)) {
        auto normLine = normalizeLine(line);
        if (isPrefix(normLine, "#pragma once")) {
            continue;
        }
        if (isPrefix(normLine, "#include")) {
            if (processInclude(normLine, repoRoot, fOut)) {
                continue;
            }
        }
        fOut->writeLine(line);
    }

    return 0;
}
