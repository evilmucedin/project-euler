#!/usr/bin/arch -i386 /opt/local/Library/Frameworks/Python.framework/Versions/2.7/Resources/Python.app/Contents/MacOS/Python

import psyco

BASE = 14

def Print(a, base):
    res = []
    while a != 0:
        res.append(a % base)
        a /= base
    return res

MAXLEN = 10000
# MAXLEN = 9

powers = [1]
for i in xrange(MAXLEN + 10):
    powers.append( powers[-1]*BASE )
print "powers..."

def IsSteady(p, base):
    sm = [0]*len(p)
    for i in xrange(len(p)):
        for j in xrange(0, min(len(p) - i + 1, len(p))):
            if i + j < len(sm):
                sm[i + j] += p[i]*p[j]
    carry = 0
    for i in xrange(len(p)):
        sm[i] += carry
        carry = sm[i] / base
        sm[i] %= base
    # print "i",
    # Draw( Print(n, base) )
    # print "ii",
    # Draw( Print(sm, base) )
    # print "ii2",
    # Draw( Print(n*n, base) )
    # print IsPrefix( p, Print(sm, base) )
    return p == sm

def Ch(x):
    if x < 10:
        return chr(x + ord('0'))
    else:
        return chr(x - 10 + ord('a'))

def Draw(p):
    if 0 == len(p):
        print "0"
    else:
        res = ""
        for i in xrange(len(p)):
            res += Ch(p[len(p) - 1 - i])
        print res

ans = []
ans.append([[]])
for ln in xrange(1, MAXLEN + 1):
    print ln,
    forTest = list(ans[ln - 1])
    if ln >= 2:
        forTest += ans[ln - 2]
    if ln >= 3:
        forTest += ans[ln - 3]
    if ln >= 4:
        forTest += ans[ln - 4]
    ans.append([])
    for a in forTest:
        for d in xrange(1, BASE):
            c = list(a)
            while len(c) != ln - 1:
                c.append(0)
            c.append(d)
            if IsSteady(c, BASE):
                # Draw(c)
                ans[-1].append(c)
    print len(ans[-1])

sm = 0
for i in xrange(MAXLEN + 1):
    for a in ans[i]:
        if len(a) == i:
            # Draw(p)
            for j in a:
                sm += j
Draw(Print(sm, BASE))
