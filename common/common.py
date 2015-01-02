#!/usr/bin/env pypy

import math
import random

def egcd(a, b):
    if 0 == b:
        return (a, 1, 0)
    else:
        gcd, sA, sB = egcd(b, a % b)
        return (gcd, sB, sA - sB*(a/b))

def modInv(a, p):
    gcd, sA, sB = egcd(a, p)
    return sA

def isPrime(n):
    if n > 1:
        top = min(int(math.sqrt(n)), n - 1)
        for i in xrange(2, top + 1):
            if n % i == 0:
                return False
        return True
    else:
        return False

def fastPowerMod(base, power, mod):
    if power == 0:
        return 1
    if 0 == (power & 1):
        base = (base*base) % mod
        power /= 2
        return fastPowerMod(base, power, mod)
    else:
        return (base*fastPowerMod(base, power - 1, mod)) % mod

def sqr(x):
    return x*x

class MillerRabin:

    def __init__(self):
        self.mPrimes = []
        N = 100
        sieve = [True]*N
        for i in xrange(2, N):
            if sieve[i]:
                self.mPrimes.append(i)
                j = i + i
                while j < N:
                    sieve[j] = False
                    j += i

    def isPrime(self, n):
        if n < 2:
            return False
        if n == 2 or n == 3:
            return True

        for p in self.mPrimes:
            if (p < n) and (n % p == 0):
                return False

        s = 0
        d = n - 1
        while 0 == (d & 1):
            d /= 2
            s += 1

        def test(witness):
            x = fastPowerMod(witness, d, n)
            if x != 1 and x != n - 1:
                for r in xrange(s - 1):
                    x = (x*x) % n
                    if 1 == x:
                        return False
                    if n - 1 == x:
                        return True
                return False
            return True

        if n > sqr(self.mPrimes[10]):
            for i in xrange(10):
                if not test(self.mPrimes[i]):
                    return False
        
        for i in xrange(10):
            witness = random.randint(2, n - 2)
            if not test(witness):
                return False

        return True


def testMillerRabin():
    N = 100000
    mr = MillerRabin()
    for i in xrange(N):
        if isPrime(i) != mr.isPrime(i):
            print i, isPrime(i), mr.isPrime(i)

import cPickle

class memoize(object):
    def __init__(self, func):
        self.func = func
        self._cache = {}
    
    def __call__(self, *args, **kwds):
        key = args
        if kwds:
            items = kwds.items()
            items.sort()
            key = key + tuple(items)
        try:
            if key in self._cache:
                return self._cache[key]
            self._cache[key] = result = self.func(*args, **kwds)
            return result
        except TypeError:
            try:
                dump = cPickle.dumps(key)
            except cPickle.PicklingError:
                return self.func(*args, **kwds)
            else:
                if dump in self._cache:
                    return self._cache[dump]
                self._cache[dump] = result = self.func(*args, **kwds)
                return result

if __name__ == "__main__":
    testMillerRabin()
