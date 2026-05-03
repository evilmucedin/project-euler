#!/usr/bin/env python

import sys

sys.path[-1] = "../common"

import common

n = 10**6
# n = 4
p = 1000000007
sum2 = 0
for k in xrange(1, n + 1):
    if (k % 10000) == 0:
        print(k)
    a = common.fastPowerMod((k*k - 1) % p, n, p)
    if 0 != (n % 2):
        a = -a
    sum2 += (a - 1)*(k*k - 1)*common.modInv(k*k % p, p)
print(sum2 % p)
