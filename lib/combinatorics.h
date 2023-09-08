#pragma once

#include "header.h"

template<typename T=ui64>
vector<vector<T>> pascalTriangle(int n) {
    vector<vector<T>> result(n);
    result[0] = {1};
    for (int i = 1; i < n; ++i) {
        result[i].resize(i + 1);
        result[i][0] = 1;
        result[i][i] = 1;
        for (int j = 1; j < i; ++j) {
            result[i][j] = result[i - 1][j] + result[i - 1][j - 1];
        }
    }
    return result;
}
