#include "lib/string.h"

#include "lib/exception.h"

bool stringToBool(const string& s) { return s == "1" || s == "true" || s == "True"; }

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

bool isPrefix(const string& s, const string& prefix) {
    if (prefix.size() > s.size()) {
        return false;
    }
    for (size_t i = 0; i < prefix.size(); ++i) {
        if (s[i] != prefix[i]) {
            return false;
        }
    }
    return true;
}

bool isSuffix(const string& s, const string& suffix) {
    if (suffix.size() > s.size()) {
        return false;
    }
    const size_t offset = s.size() - suffix.size();
    for (size_t i = 0; i < suffix.size(); ++i) {
        if (s[offset + i] != suffix[i]) {
            return false;
        }
    }
    return true;
}

string ltrim(const string& s) {
    string res = s;
    res.erase(res.begin(), std::find_if(res.begin(), res.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    return res;
}

string rtrim(const string& s) {
    string res = s;
    res.erase(std::find_if(res.rbegin(), res.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(),
              res.end());
    return res;
}

string trim(const string& s) { return ltrim(rtrim(s)); }

StringVector removeEmpty(const StringVector& ss) {
    StringVector res;
    for (const auto& s: ss) {
        if (s.size()) {
            res.emplace_back(s);
        }
    }
    return res;
}

StringVector trimAll(const StringVector& v) {
    StringVector result = v;
    for (auto& s: result) {
        s = trim(s);
    }
    return result;
}

string rep(const string& s, size_t n) {
    string result;
    for (size_t i = 0; i < n; ++i) {
        result += s;
    }
    return result;
}

string unquote(const string& s, char quote) {
    if (s.size() < 2) {
        return s;
    }
    if (s[0] == quote && s[s.size() - 1] == quote) {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

string replaceAll(const string& s, const string& from, const string& to) {
    string result = s;
    size_t index = 0;
    while (true) {
        index = result.find(from, index);
        if (index == string::npos) {
            break;
        }
        result.replace(index, from.size(), to);
        index += to.size();
    }
    return result;
}

string bytesToStr(size_t bytes) {
    static constexpr size_t GB = 1024 * 1024 * 1024;
    if (bytes >= GB) {
        return stringSprintf("%.1lfGb", static_cast<double>(bytes) / GB);
    }
    static constexpr size_t MB = 1024 * 1024;
    if (bytes >= MB) {
        return stringSprintf("%.1lfMb", static_cast<double>(bytes) / MB);
    }
    return stringSprintf("%zd", bytes);
}

void fPutWString(FILE* fOut, const WString& ws) {
    for (const auto& wch : ws) {
        if (WEOF == fputwc(wch, fOut)) {
            throw Exception("fPutWString failed");
        }
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
            result.back().emplace_back(0);
            begin = p + 1;
        }
        ++p;
    }
    if (p != begin) {
        result.emplace_back(WString(begin, p));
        result.back().emplace_back(0);
    }
    return result;
}

WString unquote(const WString& s, WChar quote) {
    if (s.size() < 2) {
        return s;
    }
    if (s[0] == quote && s[s.size() - 1] == quote) {
        WString result(s.data() + 1, s.data() + s.size() - 1);
        result.emplace_back(0);
        return result;
    }
    if (s.size() < 3) {
        return s;
    }
    if (s[0] == quote && s[s.size() - 1] == 0 && s[s.size() - 2] == quote) {
        WString result(s.data() + 1, s.data() + s.size() - 2);
        result.emplace_back(0);
        return result;
    }
    return s;
}

u64 wStringToU64(const WString& s) {
    if (s.empty()) {
        throw Exception("Empty string");
    }

    u64 result = 0;
    for (ssize_t i = s.size() - 1; i >= 0; --i) {
        if (s[i] == 0 && (i + 1 == s.size())) {
            continue;
        }
        if (s[i] < '0' || s[i] > '9') {
            THROW("Non-digit character '" << static_cast<char>(s[i]) << "'");
        }
        result = 10 * result + (s[i] - '0');
    }
    return result;
}

WString stringToWString(const string& s) { return {s.begin(), s.end()}; }
string wstringToString(const WString& ws) { return {ws.begin(), ws.end()}; }

WString& operator+=(WString& s, WChar wch) {
    s.emplace_back(wch);
    return s;
}
