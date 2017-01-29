#!/usr/bin/env python

def solve(m, n):
    result = 0
    power = 2
    n /= 2
    while power <= n:
        result += n / power
        power *= 2
    return result*(m + 1)

print solve(904961, 8)
print solve(904961, 10**12)
