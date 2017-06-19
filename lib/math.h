#pragma once

template<typename T>
T Sqr(T x) {
    return x*x;
}

template<typename T>
T power(T a, T b) {
    T result = 1;
    while (b > 0) {
        if (b & 1) {
            result *= a;
        }
        a *= a;
        b >>= 1;
    }
    return result;
}

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
T gcd(T n, T m) {
    while (m) {
        T temp = n;
        n = m;
        m = temp % m;
    }
    return n;
}

/*
template<typename T>
T egcd(T x, T y, T* ao, T* bo) {
    if (x < 0) {
        x = -x;
    }
    if (y < 0) {
        y = -y;
    }
    if (x < y) {
        T temp = x;
        x = y;
        y = temp;
    }

    T g = 0;
    while ((0 == (x & 1)) && (0 == (y & 1))) {
        x >>= 1;
        y >>= 1;
        ++g;
    }

    T u = x;
    T v = y;
    T a = 1;
    T b = 0;
    T c = 0;
    T d = 1;

    while (u != 0) {
        while (0 == (u & 1)) {
            u >>= 1;
            if ((0 != (a & 1)) || (0 != (b & 1))) {
                a += y;
                b -= x;
            }
            a >>= 1;
            b >>= 1;
        }

        while (0 == (v & 1)) {
            v >>= 1;
            if ((0 != (c & 1)) || (0 != (d & 1))) {
                c += y;
                d -= x;
            }
            c >>= 1;
            d >>= 1;
        }

        if (u >= v) {
            u -= v;
            a -= c;
            b -= d;
        } else {
            v -= u;
            c -= a;
            d -= b;
        }
    }
    *ao = c;
    *bo = d;
    return v << g;
}
*/

template<typename T>
T egcd(T n, T m, T* a, T* b) {
    if (0 == m) {
        *a = 1;
        *b = 0;
        return n;
    }
    T as;
    T bs;
    T s = egcd(m, n % m, &as, &bs);
    *a = bs;
    T q = n / m;
    *b = as - bs*q;
    assert(s == n*(*a) + m*(*b));
    return s;
}
/*
*/

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
