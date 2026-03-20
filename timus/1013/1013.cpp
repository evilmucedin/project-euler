#include <iostream>
using namespace std;
typedef long long ll;

struct Matrix {
    ll a[2][2];
    Matrix() { a[0][0] = a[0][1] = a[1][0] = a[1][1] = 0; }
};

Matrix mul(Matrix A, Matrix B, ll mod) {
    Matrix C;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
                C.a[i][j] = (C.a[i][j] + (__int128)A.a[i][k] * B.a[k][j]) % mod;
    return C;
}

Matrix mpow(Matrix A, ll p, ll mod) {
    Matrix R;
    R.a[0][0] = R.a[1][1] = 1;
    while (p > 0) {
        if (p & 1) R = mul(R, A, mod);
        A = mul(A, A, mod);
        p >>= 1;
    }
    return R;
}

int main() {
    ll n, k, m;
    cin >> n >> k >> m;

    // Count N-digit base-K numbers (first digit 1..K-1) with no two consecutive zeros.
    // Let p(j) = valid j-length sequences (digits 0..K-1) with no two consecutive zeros.
    // p(0)=1, p(1)=K, p(j)=(K-1)*p(j-1)+(K-1)*p(j-2)
    // Answer = (K-1)*p(N-1) % M

    if (n == 1) {
        cout << (k - 1) % m << endl;
        return 0;
    }

    // Matrix form: [p(j), p(j-1)]^T = T * [p(j-1), p(j-2)]^T
    // T = [(K-1), (K-1); 1, 0]
    // Apply T^(N-2) to [p(1), p(0)] = [K, 1] to get [p(N-1), ...]
    Matrix T;
    T.a[0][0] = (k - 1) % m;
    T.a[0][1] = (k - 1) % m;
    T.a[1][0] = 1;
    T.a[1][1] = 0;

    Matrix Tn = mpow(T, n - 2, m);
    ll pn1 = (Tn.a[0][0] * (k % m) + Tn.a[0][1]) % m;
    cout << (ll)((__int128)((k - 1) % m) * pn1 % m) << endl;

    return 0;
}
