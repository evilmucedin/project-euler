#include "lib/header.h"

int main() {
    I32Vector v;
    for (int i = 0; i < 3; ++i) {
        int d;
        scanf("%d", &d);
        v.emplace_back(d);
    }

    int count = 0;
    int dd;
    while (1 == scanf("%d", &dd)) {
        v.emplace_back(dd);
        if (v.back() > v[v.size() - 4]) {
            ++count;
        }
    }
    cout << count << endl;
    return 0;
}
