#!/usr/bin/env python

import sys
import math

cache = {}

def winner_(x, y):
    if x > y:
        return winner(y, x)
    if x == 0:
        return 2
    if y % x == 0:
        return 1
    y1 = y - x
    while y1 > 0:
        if winner(x, y1) == 2:
            return 1
        y1 -= x
    return 2

def winner(x, y):
    pair = (x, y)
    if not pair in cache:
        cache[pair] = winner_(x, y)
    return cache[pair]

N = 10
TAU = (1. + math.sqrt(5))/2

for y in xrange(1, N + 1):
    smallx = y
    for x in xrange(1, y):
        if winner(x, y) == 2:
            # print y, x
            if smallx == y:
                smallx = x
    print y, y - 1 - smallx + 1, int(y/(TAU + 1.))

sys.exit(0)

sm = 0
for x in xrange(1, N + 1):
    maxy = x
    for y in xrange(x + 1, N + 1):
        if winner(x, y) == 2:
            sm += x + y
            maxy = y
            print x, y
    est = int(x / TAU)
    print x, x + 1, maxy, maxy - x, est
    # assert maxy - x == est
print sm
