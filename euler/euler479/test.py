#!/usr/bin/env python
sum = 0
for k in xrange(1, 5):
    for p in xrange(1, 5):
        sum += (1 - k*k)**p
print(sum)
n = 10**6
sum2 = 0
for k in xrange(1, n + 1):
    if (k % 100) == 0:
        print(k)
    sum2 += ( 1 - (1 - k*k)**n )*(1 - k*k) / k / k
print(sum2 % 1000000007)
