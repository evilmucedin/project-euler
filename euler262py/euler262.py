#!/usr/bin/env python

import theano

x = theano.tensor.dscalar()
y = theano.tensor.dscalar()
h = (5000 - 0.005*(x*x+y*y+x*y) + 12.5*(x+y))*theano.tensor.exp( -abs(0.000001*(x*x+y*y)-0.0015*(x+y)+0.7) )
fh = theano.function([x, y], h)
gradH = theano.gradient.grad(h, [x, y])
gradHX = theano.function([x, y], gradH[0])
gradHY = theano.function([x, y], gradH[1])
print fh(0, 0), gradHX(0, 0), gradHY(0, 0)

