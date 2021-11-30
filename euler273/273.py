#!/usr/bin/env python

import math

erato = [True]*150
primes = []
for i in xrange(2, 150):
    if erato[i]:
        if i % 4 == 1:
            primes.append(i)
        j = i + i
        while j < 150:
            erato[j] = False
            j += i
print primes
print len(primes)
print 2**len(primes)
mul = 1
for p in primes:
    mul *= p
print int(math.sqrt(mul/2))
