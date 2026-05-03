#!/usr/bin/env python

import math

N = 15
N = 1000000000
top = int(math.sqrt(N)) + 1
result = 0
for n in xrange(1, N + 1):
    if 0 == (n % 100):
        print n, top, result
    for m in xrange(n, N/n + 1):
        if m*n <= N:
            a = m*n
            b = n*n
            c = m*m - n*n
            if a + b > c and a + c > b and b + c > a:
                # print a, b, c
                cnt = N/a
                result += a*cnt*(cnt + 1)/2
print(result)
