#include "ols.h"

DoubleVector LinearRegressor::mul(const DoubleMatrix& m, const DoubleVector& v) {
    DoubleVector result(v.size());
    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = 0; j < v.size(); ++j) {
            result[i] += m[i][j] * v[j];
        }
    }
    return result;
}
DoubleMatrix LinearRegressor::inverse(const DoubleMatrix& m) {
    const auto n = m.size();
    vector<DoubleVector> g(n, DoubleVector(2 * n));
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            g[i][j] = m[i][j];
        }
    }
    for (size_t i = 0; i < n; ++i) {
        g[i][n + i] = 1.0;
    }

    auto swapLines = [&](size_t i, size_t j) {
        if (i != j) {
            for (size_t k = 0; k < 2 * n; ++k) {
                swap(g[i][k], g[j][k]);
            }
        }
    };

    auto mulLine = [&](size_t i, double m) {
        for (size_t j = 0; j < 2 * n; ++j) {
            g[i][j] *= m;
        }
    };

    auto subLine = [&](size_t i, size_t j, double m) {
        for (size_t k = 0; k < 2 * n; ++k) {
            g[j][k] -= g[i][k] * m;
        }
    };

    for (size_t i = 0; i < n; ++i) {
        double mx = -1e12;
        size_t bestJ = i;
        for (size_t j = i; j < n; ++j) {
            if (abs(g[j][i]) > mx) {
                mx = abs(g[j][i]);
                bestJ = j;
            }
        }
        swapLines(i, bestJ);
        if (g[i][i]) {
            mulLine(i, 1.0 / g[i][i]);
            for (size_t j = i + 1; j < n; ++j) {
                subLine(i, j, g[j][i]);
            }
        }
    }

    for (int i = (int)n - 1; i >= 0; --i) {
        for (int j = 0; j < i; ++j) {
            subLine(i, j, g[j][i]);
        }
    }

    // cerr << g << endl;

    auto result = createMatrix(n);
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            result[i][j] = g[i][n + j];
        }
    }
    return result;
}

LinearRegressor::LinearRegressor(size_t numFeatures)
    : numFeatures_(numFeatures), xtx_(createMatrix(numFeatures_)), xty_(createVector(numFeatures_)) {}

void LinearRegressor::addSample(const DoubleVector& x, double y) {
    for (size_t j = 0; j < numFeatures_; ++j) {
        for (size_t k = 0; k < numFeatures_; ++k) {
            xtx_[j][k] += x[j] * x[k];
        }
        xty_[j] += x[j] * y;
    }
}

void LinearRegressor::l2Regularization(double lambda1, double lambda2) {
    for (size_t i = 0; i < numFeatures_; ++i) {
        const double xii = xtx_[i][i];
        if (abs(xii) > 1e-8) {
            for (size_t j = 0; j < numFeatures_; ++j) {
                xtx_[i][j] /= xii;
            }
            xty_[i] /= xii;
        }
        xtx_[i][i] = xtx_[i][i] * (1.0 + lambda2) + lambda1;
    }
}

DoubleVector LinearRegressor::regress() {
    auto ixtx = inverse(xtx_);
    beta_ = mul(ixtx, xty_);
    return beta_;
}

double LinearRegressor::apply(const DoubleVector& beta, const DoubleVector& x) const {
    double result = 0;
    for (size_t i = 0; i < numFeatures_; ++i) {
        result += beta[i] * x[i];
    }
    return result;
}

double LinearRegressor::get(const DoubleVector& x) const {
    double result = 0;
    for (size_t i = 0; i < numFeatures_; ++i) {
        result += beta_[i] * x[i];
    }
    return result;
}
