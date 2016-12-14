#!/usr/bin/env python

import theano

x = theano.tensor.dscalar()
y = theano.tensor.dscalar()
h = theano.function([x, y], (5000 - 0.005*(x*x+y*y+x*y) + 12.5*(x+y))*theano.tensor.exp( -abs(0.000001*(x*x+y*y)-0.0015*(x+y)+0.7) ))
gradH = theano.gradient.grad(h, [x, y])
print h(0, 0), gradH[0](0, 0), gradH[1](0, 0)

