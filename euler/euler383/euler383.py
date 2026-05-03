#!/usr/bin/env python

import sys

if False:
    for p in (5, 25):
        for i in xrange(0, p + 10):
            print i, p, (2*i - 1)/p - 2*(i/p)

    sys.exit(0)

def sum(i):
    power = 5
    res = 0
    while power <= i:
        res += i/power
        power *= 5
    return res

def mod5(i):
    if 0 != i:
        return mod5(i/5) + chr(i % 5 + ord('0'))
    else:
        return ""

def mod5_2(i, k):
    if 0 != i:
        if i % 5 != k:
            return mod5_2(i/5, k) + chr(i % 5 + ord('0'))
        else:
            return mod5_2(i/5, k)
    else:
        return ""

def num5(i, k):
    if 0 != i:
        return num5(i/5, k) if i % 5 != k else 1 + num5(i/5, k)
    else:
        return 0

N = 1000000
if False:
    count = 0
    for i in xrange(1, N + 1):
        if num5(2*i - 1, 4) >= num5(i, 0):
            count += 1
    print count
    count = 0

count = 0
for i in xrange(1, N + 1):
    # if sum(2*i - 1) < 2*sum(i):
    #     print "!",
    # print mod5(i/5), mod5((2*i - 1)/5), i, 2*i - 1
    if False:
        # print sum(2*i - 1), 2*sum(i)
        for j in xrange(1, 5):
            print "", 2*(i/(5 ** j)), (2*i - 1)/(5 ** j)
    if sum(2*i - 1) < 2*sum(i):
        count += 1
        # print i, mod5(i), num5(i, 0)
    d = sum(2*i) - 2*sum(i)
    nz = 0
    j = 2*i
    while j % 5 == 0:
        j /= 5
        nz += 1
    good = sum(2*i - 1) < 2*sum(i)
    assert good == (d < nz)
    print mod5(i), mod5(2*i - 1), good, mod5(sum(i)), mod5(sum(2*i - 1)), d, mod5(2*i), d < nz
print count, count2
