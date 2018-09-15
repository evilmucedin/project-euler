#pragma once

#include "lib/header.h"

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
