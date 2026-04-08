#!/usr/bin/env python

# Q = 10000
# P = 3
# PPOWER = 20

Q = 10**7
P = 61
PPOWER = 10

s = [290797]
for i in xrange(Q):
    s.append( s[-1]*s[-1] % 50515093 )

def getT(n):
    if n > Q:
        return 0
    else:
        return s[n] % P

ppower = 1
for i in xrange(PPOWER):
    ppower *= P

res = 0
for i in xrange(1, Q + 1000):
    if (i % 10000) == 0:
        print i
    sm = 0
    power = 1
    for j in xrange(PPOWER):
        sm += getT(i + j)*power
        power *= P
    res += sm
print res % ppower
