#include "lib/header.h"
#include "lib/random.h"

constexpr u32 kDim = 100;

DoubleVector genPlane(u32 dim) {
    DoubleVector result(dim);
    for (auto& x: result) {
        x = rand01<double>();
    }
    return result;
}

int main() {
    auto plane = genPlane(kDim + 1);

    return 0;
}
