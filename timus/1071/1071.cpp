#include <iostream>
#include <vector>

using namespace std;

using IntVector = vector<int>;

IntVector toRadix(int x, int radix) {
    IntVector result;
    while (x) {
        result.emplace_back(x % radix);
        x /= radix;
    }
    return result;
}

bool canMake(int x, int y, int radix) {
    IntVector xr = toRadix(x, radix);
    IntVector yr = toRadix(y, radix);

    size_t j = 0;
    size_t i = 0;
    while (j < yr.size()) {
        while (i < xr.size() && xr[i] != yr[j]) {
            ++i;
        }
        if (i == xr.size()) {
            return false;
        }
        ++j;
        ++i;
    }

    return true;
}

int main(int argc, char* argv[]) {
    int x;
    int y;

    cin >> x >> y;

    int radix = 2;
    static constexpr int LIMIT = 2000000;

    while (radix < LIMIT && !canMake(x, y, radix)) {
        ++radix;
    }

    if (radix == LIMIT) {
        cout << "No solution" << endl;
    } else {
        cout << radix << endl;
    }

    return 0;
}
