#pragma once

#include "lib/header.h"

#include <sstream>

StringVector split(const string& s, char delim = '\t');

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

template <typename U, typename V = string>
U stringCast(const V& x) {
    stringstream ss;
    ss << x;
    U result;
    ss >> result;
    return result;
}

string rep(const string& s, size_t n);

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
