#pragma once

#include <cmath>
#include <cassert>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

using u32 = uint32_t;
using i32 = int32_t;
using u64 = uint64_t;
using i64 = int64_t;
#ifdef __SIZEOF_INT128__
using u128 = unsigned __int128;
using i128 = __int128;
static_assert(sizeof(u128) == 16, "128-bit types");
static_assert(sizeof(i128) == 16, "128-bit types");
#else
using u128 = unsigned long long int;
using i128 = long long int;
static_assert(sizeof(u128) == 8, "128-bit types");
static_assert(sizeof(i128) == 8, "128-bit types");
#endif

using BoolVector = vector<bool>;
using IntVector = vector<int>;
using I32Vector = vector<i32>;
using U32Vector = vector<u32>;
using I64Vector = vector<i64>;
using U64Vector = vector<u64>;
using I128Vector = vector<i128>;
using U128Vector = vector<u128>;
using DoubleVector = vector<double>;
using StringVector = vector<string>;
using FloatVector = vector<float>;
using SizeTVector = vector<size_t>;

using I32Set = unordered_set<i32>;
using U64Set = unordered_set<u64>;
using U128Set = unordered_set<u128>;

#ifndef _MSC_VER
#ifdef __SIZEOF_INT128__
ostream& operator<<(ostream& o, u128 v);
ostream& operator<<(ostream& o, i128 v);
#else
#ifndef __clang__
ostream& operator<<(ostream& o, u128 v);
#endif
#endif
#endif

namespace std {
template<typename T>
struct hash<vector<T>> {
    size_t operator()(const vector<T>& v) const {
        size_t result = 0;
        hash<T> hasher;
        for (const auto& e: v) {
            result += hasher(e);
        }
        return result;
    }
};
}

template<typename T, typename A>
ostream& operator<<(ostream& o, const std::vector<T, A>& v) {
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

template<typename T1, typename T2>
ostream& operator<<(ostream& o, const std::map<T1, T2>& m) {
    o << "{";
    bool first = true;
    for (const auto& p: m) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << p;
    }
    o << "}";
    return o;
}

template<typename T1, typename T2>
ostream& operator<<(ostream& o, const std::unordered_map<T1, T2>& m) {
    o << "{";
    bool first = true;
    for (const auto& p: m) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << p;
    }
    o << "}";
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

template<typename T>
typename T::value_type average(const T& x) {
#ifndef NDEBUG
    assert(!x.empty());
#endif
    return sum(x) / x.size();
}

template<typename T>
vector<T> operator+(const vector<T>& a, const vector<T>& b) {
#ifndef NDEBUG
    assert(a.size() == b.size());
#endif
    vector<T> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] + b[i];
    }
    return result;
}

template<typename T>
vector<T> operator-(const vector<T>& a, const vector<T>& b) {
#ifndef NDEBUG
    assert(a.size() == b.size());
#endif
    vector<T> result(a.size());
    for (size_t i = 0; i < a.size(); ++i) {
        result[i] = a[i] - b[i];
    }
    return result;
}

template <typename T>
vector<T> cat(const vector<T>& a, const vector<T>& b) {
    vector<T> result = a;
    result.insert(result.end(), b.begin(), b.end());
    return result;
}

template<typename T>
T sqr(const T& x) {
    return x*x;
}

template<typename T>
T clamp(T x, T scale) {
    if (x < -scale) {
        return -scale;
    } else if (x > scale) {
        return scale;
    }
    return x;
}

template<typename T>
double length(const vector<T>& a) {
    double l2 = 0.0;
    for (const auto& x: a) {
        l2 += sqr(x);
    }
    return sqrt(l2);
}

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

template <typename T>
T subVector(const T& vct, size_t start) {
    return T(vct.begin() + start, vct.end());
}

template <typename T>
T subVector(const T& vct, size_t start, size_t end) {
    return T(vct.begin() + start, vct.begin() + end);
}

template <typename T>
void sort(T& v) {
    std::sort(v.begin(), v.end());
}

template <typename T>
T sorted(const T& x) {
    T copy(x);
    sort(copy);
    return copy;
}

template <typename T>
void sortAndUnique(vector<T>& v) {
    sort(v);
    v.erase(unique(v.begin(), v.end()), v.end());
}

template <typename K, typename V>
V findWithDefault(const unordered_map<K, V>& map, const K& key, const V& value) {
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    return value;
}

template <typename K, typename V>
vector<K> keys(const unordered_map<K, V>& map) {
    vector<K> result(map.size());
    size_t index = 0;
    for (const auto& it : map) {
        result[index++] = it.first;
    }
    return result;
}

class Exception : public std::exception {
   public:
    Exception();
    Exception(string msg);
    const char* what() const noexcept override;

   protected:
    string msg_;
};

#define ENFORCE(X)                                                                                    \
    {                                                                                                 \
        if (!(X)) {                                                                                   \
            throw Exception(string(__FILE__) + " " + string(to_string(__LINE__)) + " " + string(#X)); \
        }                                                                                             \
    }

#define ENFORCE_EQ(X, Y) ENFORCE((X) == (Y))

string homeDir();

#define OUT(X) #X << ": " << X << ", "
#define OUTLN(X) #X << ": " << endl << X << endl

#define EVERY_MS(LAMBDA, DELAY_MS)                                            \
    {                                                                         \
        static ::std::chrono::milliseconds::rep prev = -1;                    \
        auto now = ::std::chrono::duration_cast<::std::chrono::milliseconds>( \
                       ::std::chrono::system_clock::now().time_since_epoch()) \
                       .count();                                              \
        if (prev == -1 || now > prev + DELAY_MS) {                            \
            LAMBDA;                                                           \
            prev = now;                                                       \
        }                                                                     \
    }

#ifndef NDEBUG
#define ALWAYS_ASSERT(...) assert(__VA_ARGS__)
#else
#define ALWAYS_ASSERT(...) __VA_ARGS__
#endif

#define ASSERT          assert
#define ASSERTEQ(x, y) assert((x) == (y))
