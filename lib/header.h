#pragma once

#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>

using namespace std;

using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
using u128 = unsigned __int128;
using i128 = __int128;

using BoolVector = vector<bool>;
using IntVector = vector<int>;
using I32Vector = vector<i32>;
using U32Vector = vector<u32>;
using I64Vector = vector<i64>;
using U64Vector = vector<u64>;
using I128Vector = vector<i128>;
using U128Vector = vector<u128>;

using I32Set = unordered_set<i32>;
using U64Set = unordered_set<u64>;
using U128Set = unordered_set<u128>;

ostream& operator<<(ostream& o, u128 v);
ostream& operator<<(ostream& o, i128 v);

template<typename T>
ostream& operator<<(ostream& o, const vector<T>& v) {
    o << "[";
    bool first = true;
    for (auto n : v) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << n;
    }
    o << "]";
    return o;
}

template<typename T1, typename T2>
ostream& operator<<(ostream& o, const std::pair<T1, T2>& p) {
    o << "(" << p.first << ", " << p.second << ")";
    return o;
}

template<typename T>
typename T::value_type sum(const T& x) {
    typename T::value_type result = 0;
    for (auto v : x) {
        result += v;
    }
    return result;
}

struct Cycle {
    size_t start_;
    size_t period_;
};

ostream& operator<<(ostream& o, const Cycle& c);

template<typename T>
bool detectCycle(const T& vector, Cycle* result) {
    size_t i = 1;
    size_t j = 2;
    while ((j < vector.size()) && vector[i] != vector[j]) {
        ++i;
        j += 2;
    }
    if (j == vector.size()) {
        return false;
    }
    size_t somePeriod = j - i;
    size_t start = 0;
    while (vector[start] != vector[start + somePeriod]) {
        ++start;
    }
    result->start_ = start;
    size_t period = 1;
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

template<typename T>
typename T::value_type getFromCycle(const T& vector, const Cycle& c, size_t index) {
    if (index < vector.size()) {
        return vector[index];
    }
    return vector[(index - c.start_) % c.period_ + c.start_];
}

template<typename T>
T subVector(const T& vct, size_t start) {
    return T(vct.begin() + start, vct.end());
}

template<typename T>
T subVector(const T& vct, size_t start, size_t end) {
    return T(vct.begin() + start, vct.begin() + end);
}

template<typename T>
void sortAndUnique(vector<T>& v) {
    sort(v.begin(), v.end());
    v.erase(unique(v.begin(), v.end()), v.end());
}

bool oneIn(int n);
