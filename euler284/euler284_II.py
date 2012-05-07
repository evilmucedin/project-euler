#!/usr/bin/env python

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

def Mul(a, b, base):
    sm = [0]*len(a)
    for i in xrange(len(a)):
        for j in xrange(0, min((len(a) - i, len(b)))):
            sm[i + j] += a[i]*b[j]
    carry = 0
    for i in xrange(len(sm)):
        sm[i] += carry
        carry = sm[i] / base
        sm[i] %= base
    return sm

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
ans.append([[1]])
ans.append([[7], [8]])
for ln in xrange(2, MAXLEN + 1):
    print ln

    r1 = ans[-1][0]
    r2 = ans[-1][1]
    ans.append([])
    
    mul = [0]*ln
    mul[0] = 1
    for i in xrange(2):
        mul = Mul(mul, r1, BASE)
    ans[-1].append(mul)
    
    mul = [0]*ln
    mul[0] = 1
    for i in xrange(7):
        mul = Mul(mul, r2, BASE)
    ans[-1].append(mul)

sm = 0
for i in xrange(MAXLEN + 1):
    for a in ans[i]:
        if a[-1] != 0:
            # Draw(p)
            for j in a:
                sm += j
Draw(Print(sm, BASE))
