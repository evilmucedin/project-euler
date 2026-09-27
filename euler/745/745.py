#!/usr/bin/env python3

MOD = 10**9

def sum_of_squares_of_divisors(n):
    # Create an array to store the sum of squares of divisors
    g = [0] * (n + 1)
    
    # Fill the array using the divisor summatory technique
    for i in range(1, n + 1):
        for j in range(i, n + 1, i):
            g[j] = (g[j] + i * i) % MOD
    
    # Compute the sum of g(k) for k from 1 to n
    h = sum(g) % MOD
    
    return h

# Calculate h(10^14) % 10^9
n = 10**14
result = sum_of_squares_of_divisors(n)
print(result)

