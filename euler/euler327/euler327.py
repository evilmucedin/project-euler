#!/usr/bin/env python

def m(n, k):
    if n + 1 < k:
        return n + 1
    if n == k - 1:
        return k
    else:
        old = m(n - 1, k)
        res = 0
        res += k
        old -= k - 1
        if old % (k - 2) == 0:
            res += old/(k - 2)*k
        else:
            res += old/(k - 2)*k + (old % (k - 2) + 2)
        return res

sum = 0
for c in xrange(3, 41):
    sum += m(30, c)
print sum
