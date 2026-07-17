#include "../header.h"

// LeetCode 3700. Number of ZigZag Arrays II
//
// Count length-n arrays with values in [l, r] (inclusive) such that adjacent
// elements differ and no three consecutive elements are monotone (a strict
// zigzag). Constraints: 3 <= n <= 1e9, 1 <= l < r <= 75, answer mod 1e9+7.
//
// Only m = r - l + 1 (the number of distinct values) matters, not l/r.
//
// DP over array length. Let U[v] be the number of zigzag arrays of the current
// length that end at value v with the last move being an increase, and D[v] the
// same for a decreasing last move. Appending one element:
//
//   U'[v] = sum_{u < v} D[u]      (append a larger value; previous move down)
//   D'[v] = sum_{u > v} U[u]      (append a smaller value; previous move up)
//
// Base case at length 2: U2[v] = #{u < v} = v, D2[v] = #{u > v} = m-1-v.
//
// Reflection symmetry (v -> m-1-v) gives D[v] = U[m-1-v] for all lengths, so we
// only track U, and the recurrence collapses to:
//
//   U'[v] = sum_{u < v} U[m-1-u] = sum_{w >= m-v} U[w].
//
// That is U' = M * U with M[v][w] = (w >= m-v). Compute U_n = M^(n-2) * U2 by
// matrix exponentiation, then the answer is 2 * sum(U_n).

class Solution {
public:
    int zigZagArrays(int n, int l, int r) {
        const long long MOD = 1'000'000'007LL;
        int m = r - l + 1;

        // Transition matrix M[v][w] = 1 iff w >= m - v.
        vector<vector<long long>> M(m, vector<long long>(m, 0));
        for (int v = 0; v < m; ++v) {
            for (int w = m - v; w < m; ++w) {
                M[v][w] = 1;
            }
        }

        // Raise M to the (n - 2) power.
        auto mul = [&](const vector<vector<long long>>& a,
                       const vector<vector<long long>>& b) {
            vector<vector<long long>> c(m, vector<long long>(m, 0));
            for (int i = 0; i < m; ++i) {
                for (int k = 0; k < m; ++k) {
                    if (!a[i][k]) {
                        continue;
                    }
                    long long aik = a[i][k];
                    for (int j = 0; j < m; ++j) {
                        c[i][j] = (c[i][j] + aik * b[k][j]) % MOD;
                    }
                }
            }
            return c;
        };

        // Identity matrix.
        vector<vector<long long>> result(m, vector<long long>(m, 0));
        for (int i = 0; i < m; ++i) {
            result[i][i] = 1;
        }

        long long e = (long long)n - 2;
        while (e > 0) {
            if (e & 1) {
                result = mul(result, M);
            }
            M = mul(M, M);
            e >>= 1;
        }

        // U2[v] = v, then U_n = result * U2; answer = 2 * sum(U_n).
        long long ans = 0;
        for (int v = 0; v < m; ++v) {
            long long un = 0;
            for (int w = 0; w < m; ++w) {
                un = (un + result[v][w] * (long long)w) % MOD;
            }
            ans = (ans + un) % MOD;
        }
        ans = (ans * 2) % MOD;
        return (int)ans;
    }
};

int main() {
    Solution sol;
    Timer t("ZigZag II timer");

    cerr << sol.zigZagArrays(3, 4, 5) << endl;  // 2
    cerr << sol.zigZagArrays(3, 1, 3) << endl;  // 10
    cerr << sol.zigZagArrays(5, 1, 5) << endl;
    cerr << sol.zigZagArrays(1'000'000'000, 1, 75) << endl;

    return 0;
}
