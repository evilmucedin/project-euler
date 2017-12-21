#include "lib/header.h"

#include "glog/logging.h"

int main() {
    int sum = 0;
    for (int n = 1; n <= 1000; ++n) {
        IntVector v(n);
        for (int i = 1; i <= n; ++i) {
            v[i - 1] = i;
        }
        int dir = 1;
        while (v.size() != 1) {
            int i = (dir == 1) ? 0 : (v.size() - 1);
            while ((i < v.size()) && (i >= 0) && (v.size() != 1)) {
                v.erase(v.begin() + i);
                i += (dir == 1) ? 1 : -2;
            }
            dir = -dir;
        }
        cerr << n << "\t" << v[0] << endl;
        sum += v[0];
    }
    cout << OUT(sum) << endl;
    return 0;
}
