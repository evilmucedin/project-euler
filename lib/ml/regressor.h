#pragma once

#include <vector>

using namespace std;

using DoubleVector = vector<double>;
using DoubleMatrix = vector<DoubleVector>;

DoubleMatrix createMatrix(size_t n);
DoubleVector createVector(size_t n);

class IRegressor {
   public:
    virtual void fit(const DoubleMatrix& x, const DoubleVector& y) = 0;
    virtual DoubleVector regress(const DoubleMatrix& m) = 0;

    virtual ~IRegressor();
};
