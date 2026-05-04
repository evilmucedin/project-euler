#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    using T = int64_t;
    T n;
    T m;
    T  N;
    cin >> n >> m >> N;

    if (m == 0) {
        cout << 0 << endl;
    } else if (m == n) {
        cout << N << endl;
    } else {
        T res = m*(N + 1)/n;
        if (res > N) {
            res = N;
        }
        cout << res << endl;
    }


    return 0;
}
