#!/usr/bin/env python

N = 1000000000
d = 0
count = 0
for i in xrange(1, N + 1):
    if i % 1000000 == 0:
        print i, count
    j = i
    while j % 5 == 4:
        d -= 1
        j /= 5
    j = i
    while j % 5 == 2:
        d += 1
        j /= 5
    nz = 0
    j = i
    while j % 5 == 0:
        nz += 1
        j /= 5
    if d < nz:
        count += 1
        # print count, i
print count
