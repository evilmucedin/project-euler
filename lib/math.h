#pragma once

template<typename T>
T powerMod(T a, T b, T modulo) {
    a %= modulo;
    T result = 1;
    while (b > 0) {
        if (b & 1) {
            result = (result * a) % modulo;
        }
        a = (a*a) % modulo;
        b >>= 1;
    }
    return result;
}

template<typename T>
T power(T a, T b) {
    T result = 1;
    while (b > 0) {
        if (b & 1) {
            result *= a;
        }
        a = a*a;
        b >>= 1;
    }
    return result;
}

template<typename T>
T egcd(T n, T m, T* a, T* b) {
    if (0 == m) {
        *a = 1;
        *b = 0;
        return n;
    }
    T as;
    T bs;
    T s = EGCD(m, n % m, &as, &bs);
    *a = bs;
    *b = as - bs*(n / m);
    return s;
}

template<typename T>
T invertModPrime(T x, T prime) {
    T a;
    T b;
    T s = egcd(x, prime, &a, &b);
    while (a < 0) {
        a += prime;
    }
    while (a > prime) {
        a -= prime;
    }
    return a;
}

