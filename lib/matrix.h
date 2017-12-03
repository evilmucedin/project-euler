#pragma once

#include "lib/header.h"

using Vector = DoubleVector;

struct Matrix {
    Matrix(u32 dim) : dim_(dim), data_(dim_, Vector(dim_)) {}
    Matrix(const Matrix& m) : dim_(m.dim_), data_(m.data_) {}

    Matrix invert() const;
    Matrix cholesky() const;
    Matrix transpose() const;
    Matrix operator*(const Matrix& m) const;
    Matrix operator-(const Matrix& m) const;
    Vector operator*(const Vector& m) const;
    double norm2() const;

    u32 dim_;
    using Data = vector<Vector>;
    Data data_;
};


