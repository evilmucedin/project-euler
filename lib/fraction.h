#include <cmath>
#include <cstdint>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

template <typename T>
T fgcd(T a, T b) {
    if (a < 0) {
        a = -a;
    }

    while (b) {
        T temp = a;
        a = b;
        b = temp % b;
    }
    return a;
}

struct Frac {
    __int128 a;
    __int128 b;

    Frac() : a(0), b(0) {}

    Frac(__int128 aa) : a(aa), b(1) {}

    Frac(__int128 aa, __int128 bb) : a(aa), b(bb) {
        if (b < 0) {
            a = -a;
            b = -b;
        }
        int g = fgcd(a, b);
        a /= g;
        b /= g;
    }

    bool operator<(const Frac& x) const { return a * x.b < b * x.a; };
    bool operator==(const Frac& x) const { return a * x.b == b * x.a; }
    bool operator!=(const Frac& x) const { return !(*this == x); }
    Frac operator*(const Frac& x) const { return Frac(a * x.a, b * x.b); }
    Frac operator+(const Frac& x) const { return Frac(a * x.b + x.a * b, b * x.b); }
    Frac operator-(const Frac& x) const { return Frac(a * x.b - x.a * b, b * x.b); }
    bool operator<=(const Frac& x) const { return (*this == x) || (*this < x); }
    Frac operator-() const { return Frac(-a, b); };
};

using Pair = pair<Frac, Frac>;
using IPair = pair<__int128, __int128>;

string str(const Frac& f) {
    stringstream s;
    s << (int64_t)f.b << " " << (int64_t)f.a;
    return s.str();
}

int cmp(const IPair& a, const IPair& b, const Frac& w) {
    if (a.second == b.second) {
        if (a.first < b.first) {
            return -1;
        } else if (a.first == b.first) {
            return 0;
        } else {
            return 1;
        }
    }
    if (b.second < a.second) {
        return -cmp(b, a, w);
    }
    Frac f(a.first - b.first, b.second - a.second);
    if (f < w) {
        return -1;
    } else if (f == w) {
        return 0;
    } else {
        return 1;
    }
};

using CF = vector<__int128>;

vector<CF> toCF(const Frac& f) {
    CF result;
    auto x = f;
    while (true) {
        __int128 q = x.a / x.b;
        __int128 r = x.a % x.b;
        result.emplace_back(q);
        if (r == 0) {
            break;
        }
        x = Frac(x.b, r);
    }
    vector<CF> results;
    results.emplace_back(result);
    CF result2;
    result2.insert(result2.end(), result.begin(), result.end() - 1);
    result2.emplace_back(result.back() - 1);
    result2.emplace_back(1);
    results.emplace_back(result2);
    return results;
}

CF combine(const CF& a, const CF& b) {
    size_t i = 0;
    while ((i < a.size()) && (i < b.size())) {
        if (a[i] != b[i]) {
            CF result;
            result.insert(result.end(), a.begin(), a.begin() + i);
            result.emplace_back(min(a[i], b[i]) + 1);
            return result;
        }
        ++i;
    }
    if (i < a.size()) {
        CF result;
        result.insert(result.end(), a.begin(), a.begin() + i);
        result.emplace_back(a[i] + 1);
        return result;
    }
    if (i < b.size()) {
        CF result;
        result.insert(result.end(), b.begin(), b.begin() + i);
        result.emplace_back(b[i] + 1);
        return result;
    }
    return a;
}

Frac fromCF(const CF& a) {
    Frac result(a.back());
    for (int i = a.size() - 2; i >= 0; --i) {
        result = Frac(a[i]) + Frac(result.b, result.a);
    }
    return result;
}

