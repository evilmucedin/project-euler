#include "lib/header.h"

struct Cycle {
    u128 start_;
    u128 period_;
};

ostream& operator<<(ostream& o, const Cycle& c);

template <typename T>
bool detectCycle(const T& vector, Cycle* result) {
    u128 i = 1;
    u128 j = 2;
    while ((j < vector.size()) && vector[i] != vector[j]) {
        ++i;
        j += 2;
    }
    if (j == vector.size()) {
        return false;
    }
    u128 somePeriod = j - i;
    u128 start = 0;
    while (vector[start] != vector[start + somePeriod]) {
        ++start;
    }
    result->start_ = start;
    u128 period = 1;
    while (vector[start] != vector[start + period]) {
        ++period;
    }
    result->period_ = period;
#ifndef NDEBUG
    for (size_t i = 0; i < period; ++i) {
        assert(vector[start] == vector[start + period]);
    }
#endif
    return true;
}

template <typename T>
typename T::value_type getFromCycle(const T& vector, const Cycle& c, u128 index) {
    if (index < vector.size()) {
        return vector[index];
    }
    return vector[(index - c.start_) % c.period_ + c.start_];
}

