#!/usr/bin/env python3

import sys

n = int(sys.argv[1])
sieve = [True]*(n + 1)
sieve[0] = sieve[1] = False
primes = []
for i in range(2, n + 1):
    if sieve[i]:
        primes.append(i)
        j = i + i
        while j <= n:
            sieve[j] = False
            j += i
print("static constexpr size_t NPRIMES = %d;" % len(primes))
print("static constexpr int[] PRIMES = {%s};" % ",".join(map(str, primes)))
