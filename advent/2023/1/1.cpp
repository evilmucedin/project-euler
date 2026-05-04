#include "advent/lib/aoc.h"
#include "lib/init.h"
#include "lib/string.h"
#include "lib/exception.h"

#include "gflags/gflags.h"
#include "glog/logging.h"

DEFINE_int32(test, 1, "test number");

using TDict = unordered_map<string, int>;

vector<int> extractDigits(const string& s, const TDict& dict) {
    vector<int> result;
    for (int i = 0; i < s.size(); ++i) {
        for (const auto& kv: dict) {
            if (s.substr(i, kv.first.size()) == kv.first) {
                result.emplace_back(kv.second);
            }
        }
    }
    return result;
}

TDict digitsDict() {
    static const std::unordered_map<std::string, int> dict = {{"0", 0}, {"1", 1}, {"2", 2}, {"3", 3}, {"4", 4},
                                                              {"5", 5}, {"6", 6}, {"7", 7}, {"8", 8}, {"9", 9}};
    return dict;
}

TDict digitsWordsDict() {
    auto result = digitsDict();
    static const std::unordered_map<std::string, int> spelledNumbers = {
        {"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5},
        {"six", 6}, {"seven", 7}, {"eight", 8}, {"nine", 9}
    };
    for (const auto& kv: spelledNumbers) {
        result[kv.first] = kv.second;
    }
    return result;
}

void first() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s: input) {
        auto digits = extractDigits(s, digitsDict());
        result += digits.front() * 10 + digits.back();
    }
    cout << result << endl;
}

void second() {
    const auto input = readInputLines();
    int result = 0;
    for (const auto& s: input) {
        auto digits = extractDigits(s, digitsWordsDict());
        result += digits.front() * 10 + digits.back();
    }
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

