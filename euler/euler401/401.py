#!/usr/bin/env python

N = 10**15
# N = 6

n = N
div = 1

i = 1
count = 1

def sum2(n):
    return (2*n*n*n + 3*n*n + n)/6

def sum2i(a, b):
    return sum2(b) - sum2(a - 1)

res = 0
while i <= n:
    d = n / i
    limit1 = n / d
    limit2 = n / (d + 1)
    # print n, i, limit2 + 1, limit1, count
    if (limit1 % 100000) == 0:
        print d, limit2 + 1, limit1, sum2i(limit2 + 1, limit1)
    res += d*sum2i(limit2 + 1, limit1)
    res %= 10**9
    i = limit1 + 1
    count += 1
print res
