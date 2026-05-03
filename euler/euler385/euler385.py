#!/usr/bin/env pypy

import math

N = 300

def add(a, b):
    return (a[0] + b[0], a[1] + b[1])

def mul(a, b):
    return (a[0]*b[0] - a[1]*b[1], a[0]*b[1] + a[1]*b[0])

def eq(a, b):
    EPS = 1e-9
    return abs(a[0] - b[0]) < EPS and abs(a[1] - b[1]) < EPS

def len(a, b):
    dx = a[0] - b[0]
    dy = a[1] - b[1]
    return math.sqrt(dx*dx + dy*dy)

def sqr(x):
    return x*x

def isSquare(n):
    a = int(math.sqrt(n))
    while a*a > n:
        a -= 1
    while a*a < n:
        a += 1
    return (a*a == n, a)

def check(x):
    return isSquare(sqr(156 + 3*sqr(x[1]) - 3*sqr(x[0])) + 36*sqr(x[0])*sqr(x[1]))

def heron(p1, p2, p3):
    l1 = len(p1, p2)
    l2 = len(p1, p3)
    l3 = len(p2, p3)
    p = (l1 + l2 + l3)/2
    return math.sqrt(p*(p - l1)*(p - l2)*(p - l3))

def stupido():
    f1 = (math.sqrt(13), 0)
    f2 = (-math.sqrt(13), 0)
    zero = (0, 0)
    for x1 in xrange(-N, N + 1):
        for y1 in xrange(-N, N + 1):
            p1 = (x1, y1)
            for x2 in xrange(x1, N + 1):
                for y2 in xrange(-N, N + 1):
                    p2 = (x2, y2)
                    for x3 in xrange(x2, N + 1):
                        for y3 in xrange(y2, N + 1):
                            p3 = (x3, y3)
                            a = (3, 0)
                            b = mul( (2., 0), add(add(p1, p2), p3) )
                            c = add( add( mul(p1, p2), mul(p2, p3) ), mul(p1, p3) )
                            v1 = add(add(mul(a, mul(f1, f1)), mul(b, f1)), c)
                            v2 = add(add(mul(a, mul(f2, f2)), mul(b, f2)), c)
                            if eq(v1, zero) and eq(v2, zero) and (y2 != y1 or y3 != y1 or y2 != y3):
                                print "!", p1, p2, p3, len(p1, p2), len(p2, p3), len(p1, p3), heron(p1, p2, p3)
                                print add(add(mul(p2, p2), mul(p1, p2)), add(mul(p1, p1), (-39., 0)))
                                print add(add(mul(p3, p3), mul(p1, p3)), add(mul(p1, p1), (-39., 0)))
                                print add(add(mul(p1, p1), mul(p2, p1)), add(mul(p2, p2), (-39., 0)))
                                print check(p1), check(p2), check(p3)

def wrong():
    sm = 0
    for x in xrange(8, N + 1, 2):
        a = isSquare(3*x*x - 156)
        if a[0]:
            print "!", x
            if (a[1] % 2) == 0:
                if a[1] <= 2*N:
                    s = (x + x/2)*a[1]/2
                    print "(%d, %d) (%d, %d) (%d, %d) %d" % (x, 0, -x/2, a[1]/2, -x/2, -a[1]/2, s)
                    sm += s
    print sm

sm = 0
for a in xrange(1, N + 1):
    for b in xrange(1, N + 1):
        c = sqr(156 + 3*sqr(b) - 3*sqr(a)) + 36*sqr(a)*sqr(b)
        root = isSquare(c)
        if root[0]:
            print "!", a, b
print sm
