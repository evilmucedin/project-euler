#include <iostream>

#include "lib/implicitTreap.h"

int main() {
    int n;
    int m;

    ImplicitTreap<int> t;

    std::cin >> n >> m;
    for (size_t i = 0; i < n; ++i) {
        int j;
        std::cin >> j;
        t.push_back(j);
    }

    for (size_t i = 0; i < m; ++i) {
        int op;
        int begin;
        int end;
        std::cin >> op >> begin >> end;

        if (op == 1) {
            t.cutAndInsert(begin - 1, end, 0);
        } else {
            t.cutAndInsert(begin - 1, end, n);
        }
    }

    std::cout << std::abs(t.at(0) - t.at(t.size() - 1)) << endl;
    for (size_t i = 0; i < n; ++i) {
        std::cout << t.at(i) << " ";
    }
    std::cout << std::endl;

    return 0;
}
