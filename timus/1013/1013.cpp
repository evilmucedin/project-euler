#include <iostream>

using namespace std;

typedef unsigned __int128 uint128; // Using __int128 to handle intermediate multiplication overflow

unsigned long long mul_mod(unsigned long long a, unsigned long long b, unsigned long long m) {
    return (unsigned long long)((uint128)a * b % m);
}

struct Matrix {
    unsigned long long mat[2][2];
    Matrix() {
        mat[0][0] = mat[0][1] = mat[1][0] = mat[1][1] = 0;
    }
};

Matrix multiply(Matrix A, Matrix B, unsigned long long m) {
    Matrix C;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
                C.mat[i][j] = (C.mat[i][j] + mul_mod(A.mat[i][k], B.mat[k][j], m)) % m;
    return C;
}

Matrix power(Matrix A, unsigned long long p, unsigned long long m) {
    Matrix res;
    res.mat[0][0] = 1; res.mat[1][1] = 1;
    while (p > 0) {
        if (p & 1) res = multiply(res, A, m);
        A = multiply(A, A, m);
        p >>= 1;
    }
    return res;
}

int main() {
    unsigned long long n, k, m;
    if (!(cin >> n >> k >> m)) return 0;

    // Transition matrix:
    // [ K-1  K-1 ]
    // [  1    0  ]
    Matrix T;
    T.mat[0][0] = (k - 1) % m;
    T.mat[0][1] = (k - 1) % m;
    T.mat[1][0] = 1 % m;
    T.mat[1][1] = 0;

    // We need the result for n digits. 
    // Initial state for 1 digit (ignoring leading zero): A1 = K-1, B1 = 0
    // Result for N digits is T^(n-1) * [K-1, 0]^T
    Matrix Tn = power(T, n - 1, m);
    
    unsigned long long An = mul_mod(Tn.mat[0][0], (k - 1) % m, m);
    unsigned long long Bn = mul_mod(Tn.mat[1][0], (k - 1) % m, m);
    
    cout << (An + Bn) % m << endl;

    return 0;
}

