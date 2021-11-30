#!/usr/bin/env pypy

import sys

sys.path[-1] = '../common'

import common

import math

def sqr(x):
    return x*x

N = 5*(10**15)

i = 1
n = sqr(i + 1) + sqr(i)
count = 0
mr = common.MillerRabin()
while n < N:
    if mr.isPrime(n):
        count += 1
    if i % 100000 == 0:
        print i, count, float(n)/N
    i += 1
    n = sqr(i + 1) + sqr(i)
print count
