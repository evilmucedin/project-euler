#include "lib/header.h"

void first() {
    I32Set s;
    i32 k;
    while (scanf("%d", &k) == 1) {
        if (s.find(2020 - k) != s.end()) {
            cout << k * (2020 - k) << endl;
        }
        s.insert(k);
    }
}

void second() {
    I32Vector v;
    i32 k;
    while (scanf("%d", &k) == 1) {
        v.emplace_back(k);
    }
    for (size_t i = 0; i < v.size(); ++i) {
        for (size_t j = i + 1; j < v.size(); ++j) {
            for (size_t k = j + 1; k < v.size(); ++k) {
                if (v[i] + v[j] + v[k] == 2020) {
                    cout << v[i]*v[j]*v[k] << endl;
                }
            }
        }
    }
}

int main() {
    // first();
    second();
    return 0;
}
