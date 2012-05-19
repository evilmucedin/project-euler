#!/usr/bin/env pypy

import math
import random

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
        N = 300
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

        for i in xrange(10):
            witness = random.randint(2, n - 2)
            if not test(witness):
                return False

        if n > sqr(self.mPrimes[10]):
            for i in xrange(10):
                if not test(self.mPrimes[i]):
                    return False
        
        return True


def testMillerRabin():
    N = 100000
    mr = MillerRabin()
    for i in xrange(N):
        if isPrime(i) != mr.isPrime(i):
            print i, isPrime(i), mr.isPrime(i)

if __name__ == "__main__":
    testMillerRabin()
