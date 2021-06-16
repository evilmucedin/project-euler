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

string rep(const string& s, size_t n) {
    string result;
    for (size_t i = 0; i < n; ++i) {
        result += s;
    }
    return result;
}

string bytesToStr(size_t bytes) {
    static constexpr size_t GB = 1024 * 1024 * 1024;
    if (bytes >= GB) {
        return stringSprintf("%zdGb", bytes / GB);
    }
    static constexpr size_t MB = 1024 * 1024;
    if (bytes >= MB) {
        return stringSprintf("%zdMb", bytes / MB);
    }
    return stringSprintf("%zd", bytes);
}
