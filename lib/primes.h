#pragma once

#include <cassert>
#include <cmath>

#include "lib/header.h"
#include "lib/math.h"

struct Erato {
    Erato(size_t n);
    bool isPrime(size_t n) const;

    BoolVector sieve_;
    IntVector primes_;
};

struct PrimeFactor {
    u64 factor_;
    u32 power_;

    PrimeFactor();
    PrimeFactor(u64 factor, u32 power);
    bool operator<(const PrimeFactor& rhs) const;
};

using PrimeFactors = vector<PrimeFactor>;

ostream& operator<<(ostream& o, const PrimeFactor& v);
ostream& operator<<(ostream& o, const PrimeFactors& v);

PrimeFactors factorization(u64 number, const Erato& erato);
u64 eulerTotient(u64 number, const Erato& erato);
U64Vector divisors(const PrimeFactors& factorization);
U64Vector divisors(u64 number, const Erato& erato);
U64Vector divisors2(u64 number);
u64 smallestDivisor(u64 number, const Erato& erato);
bool isPrime(size_t number, const Erato& erato);

template<typename T>
bool isPrime(T number) {
    if (number < 2) {
        return false;
    }
    if (number < 4) {
        return true;
    }
    if (0 == (number & 1)) {
        return false;
    }
    T limit = static_cast<T>(::sqrt(static_cast<double>(number)) + 0.1);
    T div = 3;
    while (div <= limit && (number % div)) {
        div += 2;
    }
    return div > limit;
}

template<typename T>
T invertMod(T a, T b, const Erato& erato) {
    assert(1 == gcd(a, b));
    return powerMod(a, static_cast<T>(eulerTotient(b, erato)) - 1, b);
}

struct TotientErato {
    TotientErato(u32 n);

    BoolVector sieve_;
    IntVector totient_;
};

template<typename T>
bool invertMod(T a, T n, T* result) {
    T x;
    T y;
    T gcd = egcd(a, n, &x, &y);
    if (1 != gcd) {
        return false;
    }
    *result = ((x % n) + n) % n;
#ifndef NDEBUG
    assert((((*result)*a) % n) == 1);
#endif
    return true;
}

template<typename T>
bool crtMerge(T a1, T n1, T a2, T n2, T& a3, T& n3) {
    T d = gcd(n1, n2);
    T c = a2 - a1;
    if (c % d) {
        return false;
    }
    c = (c % n2 + n2) % n2;
    c /= d;
    n1 /= d;
    n2 /= d;
    T invMod;
    if (!invertMod(n1, n2, &invMod)) {
        return false;
    }
    c *= invMod;
    c %= n2;
    c *= n1 * d;
    c += a1;
    n3 = n1 * n2 * d;
    a3 = (c % n3 + n3) % n3;
    return true;
}

template<typename T>
bool crt(const vector<T>& a, const vector<T>& n, T* result)
{
    T a1 = a[0];
    T n1 = n[0];
    for (size_t i = 1; i < a.size(); ++i) {
        T a2 = a[i];
        T n2 = n[i];
        T aa;
        T nn;
        if (!crtMerge(a1, n1, a2, n2, aa, nn)) {
            return false;
        }
        a1 = aa;
        n1 = nn;
    }
    *result = (a1 % n1 + n1) % n1;
#ifndef NDEBUG
    for (size_t i = 0; i < a.size(); ++i) {
        assert(a[i] == (*result % n[i]));
    }
#endif
    return true;
}

