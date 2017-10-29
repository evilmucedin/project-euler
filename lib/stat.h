#pragma once

#include "lib/header.h"

struct StatAccumulator {
    void add(double x) {
        ++n_;
        min_ = std::min(min_, x);
        max_ = std::max(max_, x);
        sum_ += x;
        sum2_ += sqr(x);
    }

    double average() const;
    double variance() const;
    double min() const;
    double max() const;

    static constexpr double kInf = 1e15;
    u64 n_{0};
    double min_{kInf};
    double max_{-kInf};
    double sum_{0};
    double sum2_{0};
};

struct StatAccumulator2 {
    void add(double x, double y) {
        ++n_;
        x_ += x;
        y_ += y;
        xy_ += x * y;
    }

    double covariance() const;
    double xy() const;

    u64 n_{0};
    double x_{0.0};
    double y_{0.0};
    double xy_{0.0};
};
