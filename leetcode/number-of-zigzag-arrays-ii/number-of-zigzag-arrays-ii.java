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
    private static final long MOD = 1_000_000_007L;

    public int zigZagArrays(int n, int l, int r) {
        int m = r - l + 1;

        // Transition matrix M[v][w] = 1 iff w >= m - v.
        long[][] M = new long[m][m];
        for (int v = 0; v < m; ++v) {
            for (int w = m - v; w < m; ++w) {
                M[v][w] = 1;
            }
        }

        // Identity matrix.
        long[][] result = new long[m][m];
        for (int i = 0; i < m; ++i) {
            result[i][i] = 1;
        }

        // Raise M to the (n - 2) power.
        long e = (long) n - 2;
        while (e > 0) {
            if ((e & 1) == 1) {
                result = mul(result, M, m);
            }
            M = mul(M, M, m);
            e >>= 1;
        }

        // U2[v] = v, then U_n = result * U2; answer = 2 * sum(U_n).
        long ans = 0;
        for (int v = 0; v < m; ++v) {
            long un = 0;
            for (int w = 0; w < m; ++w) {
                un = (un + result[v][w] * (long) w) % MOD;
            }
            ans = (ans + un) % MOD;
        }
        ans = (ans * 2) % MOD;
        return (int) ans;
    }

    private static long[][] mul(long[][] a, long[][] b, int m) {
        long[][] c = new long[m][m];
        for (int i = 0; i < m; ++i) {
            for (int k = 0; k < m; ++k) {
                if (a[i][k] == 0) {
                    continue;
                }
                long aik = a[i][k];
                for (int j = 0; j < m; ++j) {
                    c[i][j] = (c[i][j] + aik * b[k][j]) % MOD;
                }
            }
        }
        return c;
    }

    public static void main(String[] args) {
        Solution sol = new Solution();
        System.err.println(sol.zigZagArrays(3, 4, 5));              // 2
        System.err.println(sol.zigZagArrays(3, 1, 3));              // 10
        System.err.println(sol.zigZagArrays(5, 1, 5));
        System.err.println(sol.zigZagArrays(1_000_000_000, 1, 75));
    }
}
