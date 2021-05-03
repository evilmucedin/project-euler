#!/usr/bin/env python

A = 1504170715041707
N = 4503599627370517
AINV = 3451657199285664
LIM = 50000000

print((A*AINV*3) % N)

prev = 1e100
n = 1
sm = 0
for n in range(1, 100000000000):
    x = n*A
    md = x % N
    if md < prev:
        prev = md
        sm += md
        print(n, sm, md)
        if md < LIM:
            break

print("%d %d" % (sm, n))

pairs = []
for m in range(1, md):
    pairs.append( (m, (AINV*m) % N) )

print("...pairs generated")

pairs = sorted(pairs, key=lambda a: a[1])

print("...pairs sorted")
print(pairs[0:10])

for p in pairs:
    if p[0] < prev:
        sm += p[0]
        prev = p[0]

print(sm)
