#!/usr/bin/env python

import math

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

def hilbert(floor, index):
    if 0 == floor:
        index += 1
    a = (floor + 1)**2 / 2
    for i in xrange(index):
        if (floor + 1 + i) & 1 == 0:
            a += 4*((floor + 1)/2) + 1
        else:
            a += 1
    a += (index)*(index + 1)/2
    return a

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
