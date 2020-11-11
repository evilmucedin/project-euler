#include "lib/header.h"
#include "lib/ml/regressor.h"

struct LinearRegressor : public IRegressor {
    static DoubleVector mul(const DoubleMatrix& m, const DoubleVector& v);

    static DoubleMatrix inverse(const DoubleMatrix& m);

    LinearRegressor(size_t numFeatures);

    void addSample(const DoubleVector& x, double y);
    void fit(const DoubleMatrix& x, const DoubleVector& y) override;
    DoubleVector regress(const DoubleMatrix& m) override;

    void l2Regularization(double lambda1, double lambda2);

    DoubleVector regress();

    double apply(const DoubleVector& beta, const DoubleVector& x) const;

    double get(const DoubleVector& x) const;

    size_t numFeatures_;
    DoubleMatrix xtx_;
    DoubleVector xty_;
    DoubleVector beta_;
};
