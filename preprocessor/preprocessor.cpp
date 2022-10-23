#include "lib/header.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/io/stream.h"
#include "lib/io/utils.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_string(input, "-", "input file");
DEFINE_string(output, "-", "output file");

namespace {

string normalizeLine(const string& line) {
    return trim(line);
}

bool processInclude(const string& line, const string& root, const string& fileDir, POutputStream fOut);

bool processFile(const string& filename, const string& root, POutputStream fOut) {
    const string fileDir = getDir(getAbsolutePath(filename));
    LOG(INFO) << OUT(filename) << OUT(getAbsolutePath(filename)) << OUT(getDir(getAbsolutePath(filename)));
    PInputStream fIn = openInputStream(FLAGS_input);
    string line;
    while (fIn->readLine(line) && !fIn->eof()) {
        auto normLine = normalizeLine(line);
        if (isPrefix(normLine, "#pragma once")) {
            continue;
        }
        if (isPrefix(normLine, "#include")) {
            if (processInclude(normLine, root, fileDir, fOut)) {
                continue;
            }
        }
        fOut->writeLine(line);
    }
    return true;
}

string parseInclude(const string& line) {
    string result;
    bool in = false;
    for (char ch: line) {
        if (in) {
            if (ch == '>' || ch == '\"') {
                in = false;
            } else {
                result += ch;
            }
        } else {
            if (ch == '<' || ch == '\"') {
                in = true;
            }
        }
    }
    return result;
}

bool processInclude(const string& line, const string& root, const string& fileDir, POutputStream fOut) {
    string include = parseInclude(line);
    if (include.empty()) {
        return false;
    }
    vector<string> cands = {root + "/" + include, include, fileDir + "/" + include};
    for (const auto& cand: cands) {
        // LOG(INFO) << "Cand: " << cand;
        if (isFile(cand)) {
            processFile(cand, root, fOut);
            return true;
        }
    }
    return false;
}

}  // namespace

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    const string root = repoRoot();
    LOG(INFO) << OUT(root);
    LOG(INFO) << OUT(getAbsolutePath(FLAGS_input));
    POutputStream fOut = openOutputStream(FLAGS_output);
    processFile(FLAGS_input, root, fOut);

    return 0;
}
