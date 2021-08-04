#pragma once

#include "lib/header.h"

template <typename T = double>
struct Stat {
   public:
    void add(T x) {
        sum_ += x;
        sum2_ += sqr(x);
        ++count_;
    }

    T mean() const { return sum_ / count_; }

    T stddev() const { return sqrt(sum2_ / count_ - sqr(mean())); }

    size_t count() const { return count_; };

   private:
    T sum_{};
    T sum2_{};
    size_t count_{};
};

template <typename T>
ostream& operator<<(ostream& s, const Stat<T>& stat) {
    s << "{mean=" << stat.mean() << ", stddev=" << stat.stddev() << "}";
    return s;
}
