#!/usr/bin/env python

import itertools, functools, operator, math

primes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97]

nmax = 10 ** 16
k = 4

def f(n):
    return sum(nmax // functools.reduce(operator.mul, x, 1)
        for x in itertools.combinations(primes, n))

def c(n, m):
    return math.factorial(n) // math.factorial(m) // math.factorial(n - m)

s = 0
for i in range(k, len(primes) + 1):
    ff = f(i)
    if ff == 0:
        break
    s += (-1) ** (i - k) * c(i - 1, k - 1) * ff

print(s)
