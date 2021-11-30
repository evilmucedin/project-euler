#!/usr/bin/env python

import sys
import math

def hilbert(floor, index):
    if 0 == floor:
        index += 1
    a = (floor + 1)**2 / 2
    d1 = 1
    d2 = 4*((floor + 1)/2) + 1
    a += (d1 + d2)*(index / 2)
    begin = index - (index & 1)
    for i in xrange(begin, index):
        if (floor + 1 + i) & 1 == 0:
            a += d2
        else:
            a += d1
    a += (index)*(index - 1)/2
    return a

N0 = 71328803586048
MOD = 100000000
N = N0
n1 = 0
while N % 2 == 0:
    n1 += 1
    N /= 2
n2 = 0
while N % 3 == 0:
    n2 += 1
    N /= 3

sum = 0
for i in xrange(n1 + 1):
    
    a = 1
    for i1 in xrange(i):
        a *= 2
    
    for j in xrange(n2 + 1):
        b = a
        for j1 in xrange(j):
            b *= 3

        sum += hilbert(b - 1, N0/b - 1)
print sum % MOD

def IsSquare(n):
    s = int(math.sqrt(n))
    return s*s == n

floors = []
for i in xrange(1, 100):
    j = 0
    while j < len(floors) and not IsSquare(floors[j][-1] + i):
        j += 1
    if j < len(floors):
        floors[j].append(i)
    else:
        floors.append([i])

for f in floors:
    print f

print "----"

for j, f in enumerate(floors):
    print j, "|",
    for i in xrange(len(f) - 1):
        print f[i + 1] - f[i] - i,
    print

print "----"

for j, f in enumerate(floors):
    for i in xrange(len(f)):
        print hilbert(j, i),
    print

if False:
    for i, f in enumerate(floors):
        print (i + 1)**2/2, f,
        if len(f) > 1:
            print float(f[-1])/f[-2]
        else:
            print

    for j, f in enumerate(floors):
        print j, "|",
        for i in xrange(len(f) - 1):
            print int(math.sqrt(f[i] + f[i + 1])),
        print
