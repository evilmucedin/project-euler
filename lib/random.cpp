#include "lib/random.h"

std::mt19937_64& getRandomNumberGenerator() {
    thread_local std::mt19937_64 gen;
    return gen;
}

template <>
int randAB(int a, int b) {
    std::uniform_int_distribution<int> d(a, b);
    return d(getRandomNumberGenerator());
}

int randInt() { return randAB<int>(0, std::numeric_limits<int>::max()); }

char randChar() { return 40 + (randInt() % 87); }

std::string randString(size_t len) {
    std::string result;
    result.resize(len);
    for (char& ch : result) {
        ch = randChar();
    }
    return result;
}
