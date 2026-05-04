#include "lib/header.h"

#include "lib/ml/metrics.h"

double r2(const DoubleVector& y, const DoubleVector& yHat) {
    const double sy = average(y);

    double ssTot = 0;
    for (auto yy: y) {
        ssTot += sqr(yy - sy);
    }

    double ssRes = 0;
    for (size_t i = 0; i < y.size(); ++i) {
        ssRes += sqr(y[i] - yHat[i]);
    }

    return 1.0 - ssRes / ssTot;
}
