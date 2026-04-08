#!/usr/bin/env python

import sys
import itertools

def m1(n):
    if (n & 1) != 0:
        return -1
    else:
        return 1

def aOld(x):
    if x == 0:
        return 1
    x2 = x / 2
    a2 = a(x2)
    if (x & 1) != 0:
        if (x2 & 1) != 0:
            return -a2
        else:
            return a2
    else:
        return a2

def a(x):
    prev = 0
    count = 0
    while x != 0:
        now = x & 1
        if now == 1 and prev == 1:
            count += 1
        prev = now
        x /= 2
    return m1(count)

def s_(x):
    if x == 0:
        return 1

    md = x % 4
    x4 = x/4
    if md == 0:
        return 2*s(x4) - a(x4)
    elif md == 2:
        return 2*s(x4) + m1(x4)*a(x4)
    else:
        return 2*s(x4)

sDict = {}

def s(x):
    if x < 10000000:
        if not x in sDict:
            sDict[x] = s_(x)
        return sDict[x]
    else:
        return s_(x)

def mergeiter(i1, i2, cmp=cmp):
    left, right = ExtendedIter(i1), ExtendedIter(i2)
    while 1:
        if not left.has_next():
            while 1:
                yield right.next()
        if not right.has_next():
            while 1:
                yield left.next()
        comparison = cmp(left.peek(), right.peek())
        if comparison <= 0:
            yield left.next()
        else:
            yield right.next()


class ExtendedIter:
    def __init__(self, i):
        self._myiter = iter(i)
        self._next_element = None
        self._has_next = 0
        self._prime()


    def has_next(self):
        return self._has_next


    def peek(self):
        assert self.has_next()
        return self._next_element


    def next(self):
        if not self._has_next:
            raise StopIteration
        result = self._next_element
        self._prime()
        return result


    def _prime(self):
        try:
            self._next_element = self._myiter.next()
            self._has_next = 1
        except StopIteration:
            self.next_element = None
            self._has_next = 0

cache = {}

def solve_(a):
    # print "solve", a
    if a == 1:
        yield 0
    else:
        if (a & 1) != 0:
            res = []
            count1 = 0
            for x in solve((a - 1) / 2):
                if aOld(x) == -1:
                    res.append(4*x)
                if aOld(x) == m1(x):
                    res.append(4*x + 2)
            for x in solve((a + 1) / 2):
                if aOld(x) == 1:
                    res.append(4*x)
                if aOld(x) != m1(x):
                    res.append(4*x + 2)
            res = sorted(res)
            for x in res:
                yield x
        else:
            for x in solve(a/2):
                yield 4*x + 1
                yield 4*x + 3

def solve(a):
    if a > 100000:
        return solve_(a)
    else:
        if not a in cache:
            cache[a] = list(solve_(a))
        return iter(cache[a])

for i in xrange(1, 12):
    print list(solve(i))

sys.stdout.flush()

gCache = {}

def g(t, c):
    if t == 1:
        if c == 0:
            return 0
        else:
            print "FAIL", t, c
            assert False
    else:
        if (t & 1) != 0:

            if t in gCache and len(gCache[t]) > c:
                return gCache[t][c]

            # print t, c

            gCache[t] = []

            i = 0
            j = 0
            t1 = (t - 1)/2
            t2 = (t + 1)/2

            g(t1, min(t1 - 1, c/2 + 2))
            g(t2, min(t2 - 1, c/2 + 2))

            # print "!", t, c
            while len(gCache[t]) <= min(max(t/2 + 2, c), t - 1):
                if (i < t1) and (g(t1, i) < g(t2, j)):
                    #print "A",
                    g1 = g(t1, i)
                    ag1 = a(g1)
                    if ag1 == -1:
                        #print "1",
                        gCache[t].append(4*g1)
                    if m1(g1)*ag1 == 1:
                        #print "2",
                        gCache[t].append(4*g1 + 2)
                    i += 1
                else:
                    #print "B",
                    g2 = g(t2, j)
                    ag2 = a(g2)
                    if ag2 == 1:
                        #print "1",
                        gCache[t].append(4*g2)
                    if m1(g2)*ag2 == -1:
                        #print "2",
                        gCache[t].append(4*g2 + 2)
                    j += 1
            # print

            return gCache[t][c]
        else:
            prev = g(t/2, c/2)
            if c % 2 == 0:
                return 4*prev + 1
            else:
                return 4*prev + 3

for i in xrange(1, 12):
    for j in xrange(i):
        print g(i, j),
    print

sys.stdout.flush()

pairs = []
fa = 1
fb = 1
sum = 0
for i in xrange(44):
    fa, fb = fa + fb, fa
    pairs.append( (fa, fb) )
    print i + 2, fa, fb
#pairs.reverse()

i = 2
for fa, fb in pairs:
    res = g(fa, fb - 1)
    print i, fa, fb, res
    sum += res
    i += 1
print sum
