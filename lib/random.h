#pragma once

#include <limits>
#include <random>
#include <string>

std::mt19937_64& getRandomNumberGenerator();

template<typename T>
T randNorm01() {
    static std::normal_distribution<T> unif(0, 1);
    return unif(getRandomNumberGenerator());
}

template<typename T>
T rand01() {
    static std::uniform_real_distribution<T> unif(0, 1);
    return unif(getRandomNumberGenerator());
}

template<typename T>
T randAB(T a, T b) {
    return static_cast<T>(rand01<double>() * (b - a)) + a;
}

template<>
int randAB<>(int a, int b);

template<typename T>
T rand1() {
    static std::uniform_real_distribution<T> unif(-1, 1);
    return unif(getRandomNumberGenerator());
}

template<typename T = int>
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

template <typename T>
T shuffled(T vct) {
    shuffle(vct);
    return vct;
}

int randInt();
char randChar();
std::string randString(size_t len);
