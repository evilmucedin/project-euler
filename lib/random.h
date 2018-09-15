#pragma once

#include <random>

std::mt19937_64& getRandomNumberGenerator();

template<typename T>
T rand01() {
    static std::uniform_real_distribution<T> unif(0, 1);
    return unif(getRandomNumberGenerator());
}

template<typename T>
T randAB(T a, T b) {
    return rand01<T>() * (b - a) + a;
}

template<>
inline int randAB<>(int a, int b) {
    return static_cast<int>(randAB<double>(a, b));
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

template<typename T>
void shuffle(T& vct) {
    random_shuffle(vct.begin(), vct.end());
}
