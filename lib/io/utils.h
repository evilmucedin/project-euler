#pragma once

#include <iostream>

#include "lib/header.h"

template<typename T>
void saveVector(ostream& s, const vector<T>& vct) {
    s << vct.size();
    for (const auto& x: vct) {
        s << "\t" << x;
    }
}

template<typename T>
vector<T> loadVector(istream& s) {
    size_t size;
    s >> size;
    vector<T> result(size);
    for (size_t i = 0; i < size; ++i) {
        s >> result[i];
    }
    return result;
}
