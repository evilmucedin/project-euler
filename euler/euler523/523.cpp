#include "lib/header.h"

#include <random>

int main() {
    static constexpr size_t kM = 10;
    double ops = 0.0;

    static constexpr size_t kN = 30;
    I32Vector v(kN);
    for (size_t i = 0; i < kN; ++i) {
        v[i] = i;
    }

    for (size_t i = 0; i < kM; ++i) {
        random_shuffle(v.begin(), v.end());

        while (true) {
            size_t j = 1;
            while ( j < v.size() && v[j - 1] < v[j] ) {
                ++j;
            }
            if (j == v.size()) {
                break;
            }
            auto temp = v[j];
            v.erase(v.begin() + j);
            v.insert(v.begin(), temp);
            ++ops;
        }
    }

    cout << ops / kM << endl;

    return 0;
}
