#pragma once

#include <cmath>

class Counter {
   public:
    Counter() {}

    void bump(int64_t t, double value) { value_ += value; }

    double get(int64_t t) const { return value_; }

   private:
    double value_{};
};

class ExpDecayCounter {
   public:
    ExpDecayCounter(int64_t lambda) : lambda_(lambda) {}

    void bump(int64_t t, double value) {
        decay(t);
        value_ += value;
    }

    double get(int64_t t) {
        decay(t);
        return value_;
    }

   private:
    void decay(int64_t t) {
        if (prevT_) {
            const double mult = static_cast<double>(prevT_ - t) / lambda_;
            value_ *= exp(mult);
        }
        prevT_ = t;
    }

    int64_t lambda_;
    double value_{};
    int64_t prevT_{};
};
