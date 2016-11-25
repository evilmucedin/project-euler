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
