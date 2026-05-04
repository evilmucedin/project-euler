#include "lib/ml/regressor.h"

IRegressor::~IRegressor() {}

DoubleMatrix createMatrix(size_t n) {
    DoubleMatrix result(n, DoubleVector(n));
    return result;
}

DoubleVector createVector(size_t n) { return DoubleVector(n); }

