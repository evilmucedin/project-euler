#!/usr/bin/env python

def mod5(i):
    if 0 != i:
        return mod5(i/5) + chr(i % 5 + ord('0'))
    else:
        return ""

N = 1000000000
# N = 1000
s1 = 0
s2 = 0
count = 0
for i in xrange(1, N + 1):
    if i % 1000000 == 0:
        print i, count
    j = i
    while j % 5 == 4:
        s1 += 1
        j /= 5
    j = i
    while j % 5 == 2:
        s2 += 1
        j /= 5
    nz = 0
    j = i
    while j % 5 == 0:
        nz += 1
        j /= 5
    if s2 - s1 < nz:
        count += 1
        # print count, i
    nbad = 0
    was = False
    while j:
        d = j % 5
        if not was:
            if d >= 3:
                was = True
                nbad += 1
        else:
            if d >= 2:
                nbad += 1
            else:
                was = False
        j /= 5
    print mod5(i), s2 - s1, nbad, nz
    if nz > 0:
        assert nbad == s2 - s1
print count
