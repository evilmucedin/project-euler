#!/usr/bin/env python

BASE = 14

def Print(a, base):
    res = []
    while a != 0:
        res.append(a % base)
        a /= base
    return res

def IsPrefix(a, b):
    return b[0:len(a)] == a

MAXLEN = 10000
# MAXLEN = 9

powers = [1]
for i in xrange(MAXLEN + 10):
    powers.append( powers[-1]*BASE )
print "powers..."

def IsSteady(n, base):
    p = Print(n, base)
    sm = 0
    for i in xrange(len(p)):
        for j in xrange(0, min(len(p) - i + 1, len(p))):
            sm += p[i]*p[j]*powers[i + j]
    # print "i",
    # Draw( Print(n, base) )
    # print "ii",
    # Draw( Print(sm, base) )
    # print "ii2",
    # Draw( Print(n*n, base) )
    # print IsPrefix( p, Print(sm, base) )
    psm = Print(sm, base)
    while len(psm) < len(p):
        psm.append(0)
    return IsPrefix( p, psm )

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
ans.append([0])
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
        for d in xrange(BASE):
            c = d*powers[ln - 1] + a
            if c != a:
                if IsSteady(c, BASE):
                    p = Print(c, BASE)
                    # if len(p) == ln:
                    #    Draw(p)
                    ans[-1].append(c)
    print len(ans[-1])

sm = 0
for i in xrange(MAXLEN + 1):
    for a in ans[i]:
        p = Print(a, BASE)
        if len(p) == i:
            # Draw(p)
            for j in p:
                sm += j
Draw(Print(sm, BASE))
