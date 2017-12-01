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
