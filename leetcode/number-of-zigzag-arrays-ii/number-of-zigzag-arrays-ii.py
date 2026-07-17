# LeetCode 3700. Number of ZigZag Arrays II
#
# Count length-n arrays with values in [l, r] (inclusive) such that adjacent
# elements differ and no three consecutive elements are monotone (a strict
# zigzag). Constraints: 3 <= n <= 1e9, 1 <= l < r <= 75, answer mod 1e9+7.
#
# Only m = r - l + 1 (the number of distinct values) matters, not l/r.
#
# DP over array length. Let U[v] be the number of zigzag arrays of the current
# length that end at value v with the last move being an increase, and D[v] the
# same for a decreasing last move. Appending one element:
#
#   U'[v] = sum_{u < v} D[u]      (append a larger value; previous move down)
#   D'[v] = sum_{u > v} U[u]      (append a smaller value; previous move up)
#
# Base case at length 2: U2[v] = #{u < v} = v, D2[v] = #{u > v} = m-1-v.
#
# Reflection symmetry (v -> m-1-v) gives D[v] = U[m-1-v] for all lengths, so we
# only track U, and the recurrence collapses to:
#
#   U'[v] = sum_{u < v} U[m-1-u] = sum_{w >= m-v} U[w].
#
# That is U' = M * U with M[v][w] = (w >= m-v). Compute U_n = M^(n-2) * U2 by
# matrix exponentiation, then the answer is 2 * sum(U_n).

MOD = 1_000_000_007


class Solution:
    def zigZagArrays(self, n: int, l: int, r: int) -> int:
        m = r - l + 1

        # Transition matrix M[v][w] = 1 iff w >= m - v.
        M = [[1 if w >= m - v else 0 for w in range(m)] for v in range(m)]

        def mul(a, b):
            c = [[0] * m for _ in range(m)]
            for i in range(m):
                ai = a[i]
                ci = c[i]
                for k in range(m):
                    aik = ai[k]
                    if not aik:
                        continue
                    bk = b[k]
                    for j in range(m):
                        ci[j] = (ci[j] + aik * bk[j]) % MOD
            return c

        # Identity matrix.
        result = [[1 if i == j else 0 for j in range(m)] for i in range(m)]

        # Raise M to the (n - 2) power.
        e = n - 2
        while e > 0:
            if e & 1:
                result = mul(result, M)
            M = mul(M, M)
            e >>= 1

        # U2[v] = v, then U_n = result * U2; answer = 2 * sum(U_n).
        ans = 0
        for v in range(m):
            row = result[v]
            un = 0
            for w in range(m):
                un = (un + row[w] * w) % MOD
            ans = (ans + un) % MOD
        return (ans * 2) % MOD


if __name__ == "__main__":
    sol = Solution()
    print(sol.zigZagArrays(3, 4, 5))             # 2
    print(sol.zigZagArrays(3, 1, 3))             # 10
    print(sol.zigZagArrays(5, 1, 5))
    print(sol.zigZagArrays(1_000_000_000, 1, 75))
