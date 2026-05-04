#include "stat.h"

double StatAccumulator::average() const { return sum_ / n_; }

double StatAccumulator::variance() const { return sum2_ / n_ - sqr(average()); }

double StatAccumulator::min() const { return min_; }

double StatAccumulator::max() const { return max_; }

double StatAccumulator2::covariance() const { return xy_ / n_ - (x_ / n_) * (y_ / n_); }

double StatAccumulator2::xy() const { return xy_; }

