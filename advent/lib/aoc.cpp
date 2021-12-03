#include "aoc.h"

StringVector readInputLines() {
    StringVector result;
    string s;
    getline(cin, s);
    while (!s.empty()) {
        result.emplace_back(std::move(s));
        s.clear();
        getline(cin, s);
    }
    return result;
}
