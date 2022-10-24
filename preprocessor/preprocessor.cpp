#include "gflags/gflags.h"
#include "glog/logging.h"
#include "lib/header.h"
#include "lib/init.h"
#include "lib/io/stream.h"
#include "lib/io/utils.h"
#include "lib/string.h"

DEFINE_string(input, "-", "input file");
DEFINE_string(output, "-", "output file");

namespace {

class Preprocessor {
   public:
    Preprocessor(string root) : root_(std::move(root)) {}

    static string normalizeLine(const string& line) { return trim(line); }

    bool processFile(const string& filename, POutputStream fOut) {
        const string fileDir = getDir(getAbsolutePath(filename));
        // LOG(INFO) << OUT(filename) << OUT(getAbsolutePath(filename)) << OUT(getDir(getAbsolutePath(filename))) <<
        // OUT(root);
        PInputStream fIn = openInputStream(filename);
        string line;
        while (fIn->readLine(line) && !fIn->eof()) {
            auto normLine = normalizeLine(line);
            if (isPrefix(normLine, "#pragma once")) {
                continue;
            }
            if (isPrefix(normLine, "#include")) {
                // LOG(INFO) << "Found include '" << normLine << "' in " << filename;
                if (processInclude(normLine, fileDir, fOut)) {
                    continue;
                }
            }
            fOut->writeLine(line);
        }
        return true;
    }

    static string parseInclude(const string& line) {
        string result;
        bool in = false;
        for (char ch : line) {
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

    bool processInclude(const string& line, const string& fileDir, POutputStream fOut) {
        string include = parseInclude(line);
        if (include.empty()) {
            return false;
        }
        vector<string> cands = {root_ + "/" + include, include, fileDir + "/" + include};
        for (auto& cand : cands) {
            if (includes_.count(cand)) {
                continue;
            }

            cand = simplifyPath(cand);
            // LOG(INFO) << "Cand: " << cand;
            if (isFile(cand)) {
                processFile(cand, fOut);
                includes_.emplace(cand);
                return true;
            }
        }
        return false;
    }

   private:
    string root_;
    unordered_set<string> includes_;
};

}  // namespace

int main(int argc, char* argv[]) {
    standardInit(argc, argv);

    const string root = repoRoot();
    LOG(INFO) << OUT(root);
    LOG(INFO) << OUT(getAbsolutePath(FLAGS_input));
    Preprocessor prep(root);
    POutputStream fOut = openOutputStream(FLAGS_output);
    prep.processFile(FLAGS_input, fOut);

    return 0;
}
