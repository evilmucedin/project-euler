#!/usr/bin/env python

import math
import theano

x = theano.tensor.dscalar()
y = theano.tensor.dscalar()
h = (5000 - 0.005*(x*x+y*y+x*y) + 12.5*(x+y))*theano.tensor.exp( -abs(0.000001*(x*x+y*y)-0.0015*(x+y)+0.7) )
fh = theano.function([x, y], h)
gradH = theano.gradient.grad(h, [x, y])
gradHX = theano.function([x, y], gradH[0])
gradHY = theano.function([x, y], gradH[1])
print fh(0, 0), gradHX(0, 0), gradHY(0, 0), x.dtype
fhX0 = theano.function([y, theano.In(x, value=0)], h)
gradHX0Y = theano.function([y, theano.In(x, value=0)], gradH[1])

eps = 1e-12
left = 0.0
right = 1600.0
while True:
    mid = (left + right) / 2
    if mid == left or mid == right:
        break
    if gradHX0Y(mid) > 0:
        left = mid
    else:
        right = mid

y0 = left
h0 = fh(0, left)
print "%.20lf" % h0

hDiff = h0 - h
fhDiff = theano.function([x, y], hDiff)
print fhDiff(0, y0)

def sgn(y):
    if y < -eps:
        return -1
    else:
        return 1

def findRoot2(f, x, left, right):
    sgnLeft = sgn(f(x, left))
    while True:
        mid = (left + right) / 2
        if mid == left or mid == right:
            break
        if sgn(f(x, mid)) == sgnLeft:
            left = mid
        else:
            right = mid
    return left

def fUp(x):
    return findRoot2(fhDiff, x, 0, y0)

def fDown(x):
    return findRoot2(fhDiff, x, y0, 1800)

print fUp(100)
print fDown(873)

start = (200, 200)
finish = (1400, 1400)

dx = 1e-6

def deriv(f, x):
    return (f(x + dx) - f(x - dx)) / 2.0 / dx

def norm(v):
    return math.sqrt(v[0]*v[0] + v[1]*v[1])

def scalarProduct(v1, v2):
    return (v1[0]*v2[0] + v1[1]*v2[1])/norm(v1)/norm(v2)

def diff(v1, v2):
    return (v1[0] - v2[0], v1[1] - v2[1])

def orth(f, x, point):
    y = f(x)
    v1 = diff(point, (x, y))
    v2 = (1, deriv(f, x))
    angle = scalarProduct(v1, v2)
    return angle

def searchMax(f, left, right):
    argMax = left
    mx = f(left)
    for i in xrange(20):
        N = 100
        delta = float(right - left)/N
        for j in range(N):
            x = left + delta*j
            fx = f(x)
            if fx > mx:
                mx = fx
                argMax = x
        left = argMax - 3*delta
        right = argMax + 3*delta
    print "searchMax %lf %lf" % (right, left)
    return argMax

x1 = searchMax(lambda x: orth(fUp, x, start), 0.0, 200.0)
print "tan1", x1, orth(fUp, x1, start)
t1 = (x1, fUp(x1))

def fDownFinish(x):
    return orth(fDown, x, finish)

print "fDownFinish: %f" % fDownFinish(873)

x2 = searchMax(fDownFinish, 400.0, 1000.0)
print "tan2", x2, fDownFinish(x2)
t2 = (x2, fDown(x2))

def curvLength(f, left, right):
    N = 100000
    y = f(left)
    delta = (right - left)/N
    result = 0.0
    for i in xrange(N):
        xNext = left + (i + 1)*delta
        yNext = f(xNext)
        result += norm( (delta, yNext - y) )
        y = yNext
    return result

s1 = norm( diff(start, t1 ) )
s2 = curvLength(fUp, 0, x1)
s3 = curvLength(fDown, 0, x2)
s4 = norm( diff(finish, t2) )
print s1, s2, s3, s4
result = s1 + s2 + s3 + s4
print "%.20lf" % result
print "%.3lf" % result
