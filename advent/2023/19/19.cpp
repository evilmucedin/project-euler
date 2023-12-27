#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

struct Rule {
    string field_;
    int value_;
    string decision_;
};

struct Workflow {
    string name_;
    vector<Rule> rules_;
};

void first() {
    const auto input = readInputLinesAll();

    vector<Workflow> flows;
    int iLine = 0;
    while (iLine < input.size()) {
        const string& s = input[iLine];
        if (s.empty()) {
            break;
        }
        ++iLine;
    }

    int result = 0;
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    cout << result << endl;
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

