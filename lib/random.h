#pragma once

#include <random>

std::mt19937_64& getRandomNumberGenerator() {
    thread_local std::mt19937_64 rng;
    return rng;
}

template<typename T>
T rand01() {
    static std::uniform_real_distribution<T> unif(0, 1);
    return unif(getRandomNumberGenerator());
}

template<typename T>
T randAB(T a, T b) {
    return rand01<T>() * (b - a) + a;
}

template<typename T>
T rand1() {
    static std::uniform_real_distribution<T> unif(-1, 1);
    return unif(getRandomNumberGenerator());
}

template<typename T>
T dice(T n) {
    return getRandomNumberGenerator()() % n;
}

template<typename T>
bool oneIn(T n) {
    return 0 == dice(n);
}
