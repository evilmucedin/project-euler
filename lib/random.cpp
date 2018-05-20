#include "lib/random.h"

template<>
int randAB(int a, int b) {
    std::uniform_int_distribution<int> d(a, b);
    return d(getRandomNumberGenerator());
}

std::mt19937_64& getRandomNumberGenerator() {
    thread_local std::mt19937_64 gen;
    return gen;
}

int randInt() {
    return randAB<int>(0, std::numeric_limits<int>::max());
}
