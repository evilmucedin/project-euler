#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

vector<int> findHorMirrors(const vector<string>& field) {
    vector<int> result;
    for (int i = 1; i < field.size(); ++i) {
        int row;
        for (row = 0; row <= i; ++row) {
            if (field[row] != field[i + row]) {
                break;
            }
        }
        if (row > i) {
            result.emplace_back(i);
        }
    }
    LOG(INFO) << OUT(result);
    return result;
}

vector<int> findVertMirrors(const vector<string>& field) {
    return findHorMirrors(transpose(field));

    /*
    vector<int> result;
    for (int i = 1; i < field.size(); ++i) {
        int col;
        for (col = 0; col <= i; ++col) {
            if (field[col] != field[i + col]) {
                break;
            }
        }
        if (row > i) {
            result.emplace_back(i);
        }
    }
    return result;
    */
}

void first() {
    const auto input = readInputLinesAll();
    int result = 0;
    int begin = 0;
    for (int i = 0; i <= input.size(); ++i) {
        if (input[0].size() == 0 || i == input.size()) {
            vector<string> field;
            for (int j = begin; j < i; ++j) {
                field.emplace_back(input[j]);
            }
            begin = i + 1;
            for (int j : findHorMirrors(field)) {
                result += j;
            }
            for (int j : findVertMirrors(field)) {
                result += 100*j;
            }
        }
    }
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

