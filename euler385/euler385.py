#!/usr/bin/env pypy

import math

def isSquare(n):
    a = int(math.sqrt(n))
    while a*a < n:
        a += 1
    return (a*a == n, a)

N = 10
sm = 0
for x in xrange(-N, N + 1):
    a = isSquare(3*x*x - 156):
    if a[0]:
        print x, a[1]
