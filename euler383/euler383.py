#!/usr/bin/env python

def sum(i):
    power = 5
    res = 0
    while power <= i:
        res += i/power
        power *= 5
    return res

N = 1000
count = 0
for i in xrange(1, N + 1):
    if sum(2*i - 1) < 2*sum(i):
        count += 1
print count
