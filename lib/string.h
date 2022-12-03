#pragma once

#include "lib/header.h"

#include <sstream>

bool stringToBool(const string& s);

StringVector split(const string& s, char delim = '\t');
StringVector splitQuoted(const string& s, char delim = '\t', char quote = '\'');

template <typename T>
StringVector splitByFunctor(const string& s, T isDelimiter) {
    StringVector result;
    auto p = s.data();
    auto begin = p;
    bool in = true;
    while (*p) {
        bool delim = isDelimiter(*p);
        if (in) {
            if (delim) {
                if (p != begin) {
                    result.emplace_back(begin, p - begin);
                }
                in = false;
            }
        } else {
            if (!delim) {
                begin = p;
                in = true;
            }
        }
        ++p;
    }
    if (in) {
        if (p != begin) {
            result.emplace_back(begin, p - begin);
        }
    }
    return result;
}

string join(const string& delim, const StringVector& parts);

bool hasSubstring(const string& s, const string& substr);
bool isPrefix(const string& s, const string& prefix);
bool isSuffix(const string& s, const string& suffix);
bool isPalindrome(const string& s);

template <typename U, typename V = string>
U stringCast(const V& x) {
    stringstream ss;
    ss << x;
    U result;
    ss >> result;
    return result;
}

template <typename U, typename V = string>
vector<U> stringVectorCast(const vector<V>& v) {
    vector<U> result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        result[i] = stringCast<U, V>(v[i]);
    }
    return result;
}

template <typename T = int>
vector<T> parseIntegers(const string& s) {
    vector<T> result;
    size_t begin = 0;
    bool in = false;
    for (size_t i = 0; i <= s.size(); ++i) {
        const bool isDigit = isdigit(s[i]) || s[i] == '-';
        if (in) {
            if (!isDigit) {
                result.emplace_back(stringCast<T>(s.substr(begin, i - begin)));
                in = false;
            }
        } else {
            if (isDigit) {
                in = true;
                begin = i;
            }
        }
    }
    return result;
}

string ltrim(const string& s);
string rtrim(const string& s);
string trim(const string& s);
StringVector trimAll(const StringVector& v);
StringVector removeEmpty(const StringVector& ss);

string rep(const string& s, size_t n);
string unquote(const string& s, char quote = '\'');
string replaceAll(const string& s, const string& from, const string& to);

template <typename... Args>
string stringSprintf(const string& format, Args... args) {
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    if (size <= 0) {
        throw std::runtime_error("Error during formatting.");
    }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

string bytesToStr(size_t bytes);

using WChar = wint_t;
using WString = vector<WChar>;
using WStringVector = vector<WString>;

void fPutWString(FILE* fOut, const WString& ws);
WStringVector split(const WString& s, WChar delim = '\t');
WString unquote(const WString& s, WChar = '\'');
u64 wStringToU64(const WString& s);
WString stringToWString(const string& s);
string wstringToString(const WString& ws);

WString& operator+=(WString& s, WChar wch);
