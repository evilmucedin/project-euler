#include "lib/random.h"

std::mt19937_64& getRandomNumberGenerator() {
    thread_local std::mt19937_64 gen;
    return gen;
}
