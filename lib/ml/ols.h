#include "lib/header.h"

using DoubleVector = vector<double>;
using DoubleMatrix = vector<DoubleVector>;

DoubleMatrix createMatrix(size_t n);
DoubleVector createVector(size_t n);

struct LinearRegressor {
    static DoubleVector mul(const DoubleMatrix& m, const DoubleVector& v);

    static DoubleMatrix inverse(const DoubleMatrix& m);

    LinearRegressor(size_t numFeatures);

    void addSample(const DoubleVector& x, double y);

    void l2Regularization(double lambda1, double lambda2);

    DoubleVector regress();

    double apply(const DoubleVector& beta, const DoubleVector& x) const;

    double get(const DoubleVector& x) const;

    size_t numFeatures_;
    DoubleMatrix xtx_;
    DoubleVector xty_;
    DoubleVector beta_;
};
