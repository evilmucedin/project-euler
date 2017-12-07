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
    Matrix& operator/=(double x);
    double norm2() const;
    u32 dimension() const;

    static Matrix one(u32 dim);

    u32 dim_;
    using Data = vector<Vector>;
    Data data_;
};

struct VectorPoint {
    VectorPoint();
    VectorPoint(u32 dim);
    u32 dimension() const;
    Vector asVector() const;

    DoubleVector a_;
    double b_;
};
using VectorPoints = vector<VectorPoint>;

ostream& operator<<(ostream& o, const VectorPoint& p);

DoubleVector linearRegression(const VectorPoints& points, double rigid = 0.0);

template<typename T>
T dot(const vector<T>& a, const vector<T>& b) {
    T result{};
    assert(a.size() == b.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result += a[i]*b[i];
    }
    return result;
}
