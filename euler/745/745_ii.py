#!/usr/bin/env python3
import math

def solve_euler_745(N):
    MOD = 1000000007
    limit = math.isqrt(N)
    
    # Initialize arrays for the linear sieve
    f = [0] * (limit + 1)
    f[1] = 1
    primes = []
    is_prime = [True] * (limit + 1)
    
    # Linear sieve to calculate the multiplicative function f(k) in O(sqrt(N))
    for i in range(2, limit + 1):
        if is_prime[i]:
            primes.append(i)
            f[i] = (i * i - 1) % MOD
        for p in primes:
            if i * p > limit:
                break
            is_prime[i * p] = False
            if i % p == 0:
                # p divides i, so f(i * p) = f(i) * p^2
                f[i * p] = (f[i] * p * p) % MOD
                break
            else:
                # p is coprime to i, so f(i * p) = f(i) * f(p)
                f[i * p] = (f[i] * (p * p - 1)) % MOD
                
    # Compute the final summation
    ans = 0
    for k in range(1, limit + 1):
        term = (N // (k * k)) % MOD
        ans = (ans + term * f[k]) % MOD
        
    return ans

if __name__ == "__main__":
    n = 10**14
    result = solve_euler_745(n)
    print(result)

