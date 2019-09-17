#include "lib/string.h"

StringVector split(const string& s, char delim) {
    StringVector result;
    auto p = s.data();
    auto begin = p;
    while (*p) {
        if (*p == delim) {
            result.emplace_back(begin, p - begin);
            begin = p + 1;
        }
        ++p;
    }
    if (p != begin) {
        result.emplace_back(begin, p - begin);
    }
    return result;
}

string join(const string& delim, const StringVector& parts) {
    string result;
    bool first = true;
    for (const auto& s : parts) {
        if (!first) {
            result += delim;
        }
        result += s;
        first = false;
    }
    return result;
}

bool hasSubstring(const string& s, const string& substr) { return s.find(substr) != string::npos; }
