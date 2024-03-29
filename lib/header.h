#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <complex>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

using u8 = uint8_t;
using i8 = int8_t;
using u16 = uint16_t;
using i16 = int16_t;
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
using IntMatrix = vector<IntVector>;
using I64Matrix = vector<I64Vector>;
using U64Matrix = vector<U64Vector>;
using DoubleMatrix = vector<DoubleVector>;

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

inline void hashCombine(std::size_t& seed) {}

template <typename T, typename... Rest>
inline void hashCombine(std::size_t& seed, const T& v, Rest... rest) {
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    hashCombine(seed, rest...);
}

namespace std {
template <typename T>
struct hash<vector<T>> {
    size_t operator()(const vector<T>& v) const {
        size_t result = 0;
        for (const auto& e : v) {
            hashCombine(result, e);
        }
        return result;
    }
};

template <typename A, typename B>
struct hash<pair<A, B>> {
    size_t operator()(const pair<A, B>& v) const {
        size_t result = 0;
        hashCombine(result, v.first, v.second);
        return result;
    }
};
string to_string(const string& s);

template <typename T>
string to_string(const complex<T>& x) {
    return "(" + to_string(x.real()) + "," + to_string(x.imag()) + ")";
}

template <typename T, typename A>
string to_string(const std::vector<T, A>& v) {
    std::string s = "[";
    bool first = true;
    for (auto n : v) {
        if (!first) {
            s += ", ";
        }
        first = false;
        s += to_string(n);
    }
    s += "]";
    return s;
}

}  // namespace std

template<typename T>
int sz(const T& v) {
    return static_cast<int>(v.size());
}

template <typename T1, typename T2>
ostream& operator<<(ostream& o, const std::pair<T1, T2>& p) {
    o << "(" << p.first << ", " << p.second << ")";
    return o;
}

template <typename T, typename A>
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

template <typename T, typename C, typename A>
ostream& operator<<(ostream& o, const std::multiset<T, C, A>& v) {
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

template <typename T1, typename T2>
ostream& operator<<(ostream& o, const std::map<T1, T2>& m) {
    o << "{";
    bool first = true;
    for (const auto& p : m) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << p;
    }
    o << "}";
    return o;
}

template <typename T1, typename T2>
ostream& operator<<(ostream& o, const std::unordered_map<T1, T2>& m) {
    o << "{";
    bool first = true;
    for (const auto& p : m) {
        if (!first) {
            o << ", ";
        }
        first = false;
        o << p;
    }
    o << "}";
    return o;
}

template <typename T>
typename T::value_type sum(const T& x) {
    typename T::value_type result = 0;
    for (auto v : x) {
        result += v;
    }
    return result;
}

template <typename T>
typename T::value_type product(const T& x) {
    typename T::value_type result = 1;
    for (auto v : x) {
        result *= v;
    }
    return result;
}

template <typename T>
typename T::value_type average(const T& x) {
#ifndef NDEBUG
    assert(!x.empty());
#endif
    return sum(x) / x.size();
}

template <typename T>
typename T::value_type minV(const T& x) {
#ifndef NDEBUG
    assert(!x.empty());
#endif
    typename T::value_type res = x[0];
    for (size_t i = 1; i < x.size(); ++i) {
        if (x[i] < res) {
            res = x[i];
        }
    }

    return res;
}

template <typename T>
typename T::value_type maxV(const T& x) {
#ifndef NDEBUG
    assert(!x.empty());
#endif
    typename T::value_type res = x[0];
    for (size_t i = 1; i < x.size(); ++i) {
        if (x[i] > res) {
            res = x[i];
        }
    }

    return res;
}

template <typename T>
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

template <typename T>
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

template <typename T>
vector<T>& catInplace(vector<T>& a, const vector<T>& b) {
    a.insert(a.end(), b.begin(), b.end());
    return a;
}

template <typename T>
T sqr(const T& x) {
    return x * x;
}

template <typename T>
T clamp(T x, T scale) {
    if (x < -scale) {
        return -scale;
    } else if (x > scale) {
        return scale;
    }
    return x;
}

template <typename T>
double length(const vector<T>& a) {
    double l2 = 0.0;
    for (const auto& x : a) {
        l2 += sqr(x);
    }
    return sqrt(l2);
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
T subVector(const T& vct, const SizeTVector& indices) {
    T result(indices.size());
    size_t index = 0;
    for (auto i : indices) {
        result[index++] = vct[i];
    }
    return result;
}

template <typename V, typename T = typename V::value_type>
vector<T> slice(const V& vct, size_t start, size_t finish) {
    vector<T> result;
    result.reserve(finish - start);
    size_t i = start;
    while (i < finish && i < vct.size()) {
        result.emplace_back(vct[i]);
        ++i;
    }
    return result;
}

template <typename V, typename K = V>
void insertVector(vector<V>& v, size_t pos, const vector<K>& ins) {
    v.insert(v.begin() + pos, ins.begin(), ins.end());
}

template <typename T, typename V>
bool has(const T& v, const V& x) {
    for (const auto& i : v) {
        if (i == x) {
            return true;
        }
    }
    return false;
}

template <typename T>
void sort(T& v) {
    std::sort(v.begin(), v.end());
}

template <typename T>
void sortDescending(T& v) {
    std::sort(v.begin(), v.end(),
              [](const typename T::value_type& a, const typename T::value_type& b) { return a > b; });
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

template <typename T>
void reverse(T& v) {
    std::reverse(v.begin(), v.end());
}

template <typename T>
T reversed(const T& x) {
    T copy(x);
    reverse(copy);
    return copy;
}

template <typename T>
IntVector numToDigits(T n, int base = 10) {
    if (n == 0) {
        return {0};
    }
    IntVector result;
    while (n) {
        result.emplace_back(n % base);
        n /= base;
    }
    reverse(result);
    return result;
}

template <typename T, int BASE>
size_t numToBuffer(T n, char* buffer) {
    const char* buffer0 = buffer;
    if (n < 0) {
        *buffer = '-';
        ++buffer;
        n = -n;
    }
    if (n != 0) {
        char* begin = buffer;
        while (n) {
            const T nn = n / BASE;
            *buffer = (n - nn * BASE) + '0';
            ++buffer;
            n = nn;
        }
        reverse(begin, buffer);
    } else {
        *buffer = '0';
        ++buffer;
    }
    return buffer - buffer0;
}

template <typename T, int BASE = 10>
string numToString(T n) {
    thread_local char buffer[64];
    buffer[numToBuffer<T, BASE>(n, buffer)] = 0;
    return buffer;
}

template <typename T = int>
T digitsToNum(const IntVector& digits, int base = 10) {
    T result = 0;
    for (auto d : digits) {
        result = base * result + d;
    }
    return result;
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

template <typename M>
vector<pair<typename M::key_type, typename M::mapped_type>> map2Tuples(const M& map) {
    vector<pair<typename M::key_type, typename M::mapped_type>> result(map.size());
    size_t index = 0;
    for (const auto& it : map) {
        result[index++] = it;
    }
    return result;
}

template <typename M>
vector<pair<typename M::mapped_type, typename M::key_type>> map2TuplesSwap(const M& map) {
    vector<pair<typename M::mapped_type, typename M::key_type>> result(map.size());
    size_t index = 0;
    for (const auto& it : map) {
        result[index++] = make_pair(it.second, it.first);
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

////////////////////////////////////////////////////////////////

// Print tuple.
template <std::size_t I = 0, typename... Tp>
inline typename std::enable_if<I == sizeof...(Tp), string>::type stringval(const std::tuple<Tp...>& t) {
    std::stringstream buffer;
    buffer << "]";
    return buffer.str();
}

template <std::size_t I = 0, typename... Tp>
    inline typename std::enable_if < I<sizeof...(Tp), string>::type stringval(const std::tuple<Tp...>& t) {
    std::stringstream buffer;
    size_t len = sizeof...(Tp);
    if (I == 0) {
        buffer << "[";
    }
    buffer << std::get<I>(t);
    if (I < len - 1) {
        buffer << ", ";
    }
    buffer << stringval<I + 1, Tp...>(t);
    return buffer.str();
}

template <typename... Tp>
ostream& operator<<(ostream& out, const std::tuple<Tp...>& t) {
    out << stringval(t);
    return out;
}

template<typename T>
void doMax(T& mx, const T& val) {
    if (val > mx) {
        mx = val;
    }
}

template<typename T>
void doMin(T& mx, const T& val) {
    if (val < mx) {
        mx = val;
    }
}

#define ENFORCE(X)                                                                                    \
    {                                                                                                 \
        if (!(X)) {                                                                                   \
            throw Exception(string(__FILE__) + " " + string(to_string(__LINE__)) + " " + string(#X)); \
        }                                                                                             \
    }

#define ENFORCE_EQ(X, Y) ENFORCE((X) == (Y))

string homeDir();

#define OUT(X) #X << ": " << (X) << ", "
#define OUTLN(X) #X << ": " << endl << (X) << endl

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

#define ASSERT assert
#define ASSERTEQ(x, y) assert((x) == (y))
#define ASSERTNEQ(x, y) assert((x) != (y))
#define ASSERTLT(x, y) assert((x) < (y))
#define ASSERTLE(x, y) assert((x) <= (y))
#define ASSERTGT(x, y) assert((x) > (y))
#define ASSERTGE(x, y) assert((x) >= (y))
#define ASSERTNE(x, y) assert((x) != (y))

#define FOR(i, a, b) for (int i = (a); i < (b); ++i)
#define REP(i, n) FOR(i, 0, n)
#define ITRANGE(v) (v).begin(), (v).end()
