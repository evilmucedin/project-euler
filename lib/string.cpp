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

string unquote(const string& s) {
    if (s.size() < 2) {
        return s;
    }
    if (s[0] == s[s.size() - 1]) {
        return s.substr(1, s.size() - 2);
    }
    return s;
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

void FPutWString(FILE* fOut, const WString& ws) {
    for (const auto& wch : ws) {
        fputwc(wch, fOut);
    }
}

WStringVector split(const WString& s, WChar delim) {
    WStringVector result;
    auto p = s.data();
    auto begin = p;
    auto end = p + s.size();
    while (p < end) {
        if (*p == delim) {
            result.emplace_back(WString(begin, p));
            begin = p + 1;
        }
        ++p;
    }
    if (p != begin) {
        result.emplace_back(WString(begin, p));
    }
    return result;
}

WString unquote(const WString& s) {
    if (s.size() < 2) {
        return s;
    }
    if (s.front() == s.back()) {
        return WString(s.data() + 1, s.data() + s.size() - 1);
    }
    return s;
}

WString& operator+=(WString& s, WChar wch) {
    s.emplace_back(wch);
    return s;
}
