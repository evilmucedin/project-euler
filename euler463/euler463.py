#!/usr/bin/env python

import math

def s(n):
    power = int(math.log(n, 2) + 0.999999)

    if n == 2**power:
        return ((4**power - 1) / 3) + 1

    base = ((4**(power - 1) - 1) / 3) + 1
    n -= 2**(power-1)

    power -= 1
    bit = [0] * power
    for i in range(power - 1, -1, -1):
        if 0 != n & (2**i):
            bit[i] += (n % 2**i) + 1
            for j in range(i - 1, -1, -1):
                bit[j] += 2**(i - 1)

    base += n
    for i in range(0, power):
        base += 2**(power - i) * bit[i]
    return base

print s(3**37) % 1000000000
