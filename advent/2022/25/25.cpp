#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

long long snafuToTen(const string& s) {
    long long result = 0;
    for (auto ch : s) {
        result = 5 * result;
        switch (ch) {
            case '2':
                result += 2;
                break;
            case '1':
                result += 1;
                break;
            case '0':
                result += 0;
                break;
            case '-':
                result += -1;
                break;
            case '=':
                result += -2;
                break;
        }
    }
    return result;
}

string tenToSnafu(long long result) {
    if (result == 0) {
        return "0";
    }
    long long rem = result % 5;
    char ch;
    long long d = result / 5;
    switch (rem) {
        case 0:
            ch = '0';
            break;
        case 1:
            ch = '1';
            break;
        case 2:
            ch = '2';
            break;
        case 3:
            ch = '=';
            d += 1;
            break;
        case 4:
            ch = '-';
            d += 1;
            break;
    }
    return tenToSnafu(d) + ch;
    /*
    long long power5 = 1;
    while (5*power5 < result) {
        power5 *= 5;
    }
    return string(1, char(result / power5) + '0') + tenToSnafu(result % power5);
    */
}

void first() {
    const auto input = readInputLines();
    long long result = 0;
    for (const auto& s: input) {
        result += snafuToTen(s);
    }
    cerr << result << endl;
    cout << tenToSnafu(result) << endl;
    cerr << snafuToTen(tenToSnafu(result)) << endl;
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

