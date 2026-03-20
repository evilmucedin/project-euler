#include <iostream>
#include <vector>

using namespace std;

typedef long long ll;
typedef __int128_t int128; // Use __int128 for intermediate products

struct Matrix {
    int128 mat[2][2];
    Matrix() {
        mat[0][0] = mat[0][1] = mat[1][0] = mat[1][1] = 0;
    }
};

Matrix multiply(Matrix a, Matrix b, ll mod) {
    Matrix c;
    for (int i = 0; i  0) {
        if (p & 1) res = multiply(res, a, mod);
        a = multiply(a, a, mod);
        p >>= 1;
    }
    return res;
}

int main() {
    ll n, k, m;
    if (!(cin >> n >> k >> m)) return 0;

    if (n == 1) {
        cout << (k - 1) % m << endl;
        return 0;
    }

    Matrix T;
    T.mat[0][0] = 0; T.mat[0][1] = 1;
    T.mat[1][0] = k - 1; T.mat[1][1] = k - 1;

    T = power(T, n - 1, m);

    // Initial vector [dp[1][0], dp[1][1]] = [0, k-1]
    // Final result is (0 * T[0][0] + (k-1) * T[0][1]) + (0 * T[1][0] + (k-1) * T[1][1])
    // Simplified: (k-1) * T[0][1] + (k-1) * T[1][1]
    ll ans = ((int128)(k - 1) * T.mat[0][1] + (int128)(k - 1) * T.mat[1][1]) % m;
    cout << ans << endl;

    return 0;
}

