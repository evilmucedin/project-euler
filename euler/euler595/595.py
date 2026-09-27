#!/usr/bin/env python3

def solve_euler_595(N=20):
    # Precompute factorials
    fact = [1] * (N + 1)
    for i in range(1, N + 1):
        fact[i] = fact[i - 1] * i

    # Precompute combinations (n Cr)
    C = [[0] * (N + 1) for _ in range(N + 1)]
    for i in range(N + 1):
        C[i][0] = 1
        for j in range(1, i + 1):
            C[i][j] = C[i - 1][j - 1] + C[i - 1][j]

    # g[n] will store the number of permutations of length n 
    # where NO adjacent elements are consecutive (i.e., breaks into exactly n blocks)
    g = [0] * (N + 1)
    g[1] = 1
    
    for i in range(2, N + 1):
        # Total permutations is fact[i]. 
        # We subtract cases where there are fixed consecutive subsequences using inclusion-exclusion.
        total = fact[i]
        for j in range(1, i):
            total -= C[i - 1][i - j] * g[j]
        g[i] = total

    # E[s] stores the expected number of shuffles for a deck of size s
    E = [0.0] * (N + 1)
    E[1] = 0.0

    for n in range(2, N + 1):
        sum_expected = 0.0
        # P(n, k) is the probability that n elements split into k blocks.
        # Number of ways to choose structural cuts is C[n-1][n-k], 
        # and then those k blocks must not have any consecutive elements among them -> g[k] ways.
        for k in range(2, n + 1):
            ways = C[n - 1][n - k] * g[k]
            prob = ways / fact[n]
            sum_expected += prob * E[k]
            
        # P(n, n) is the probability of breaking into n blocks
        p_nn = g[n] / fact[n]
        
        # Add 1 to represent the cost of the current shuffle (excluding the fully sorted state)
        current_shuffle_cost = 1.0 - (1.0 / fact[n])
        
        E[n] = (current_shuffle_cost + sum_expected) / (1.0 - p_nn)

    return f"{E[N]:.8f}"

if __name__ == "__main__":
    # Project Euler 595 asks for N = 20 cards
    print(f"Answer for N=52: {solve_euler_595(52)}")

