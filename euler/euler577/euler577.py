#!/usr/bin/env python

def h(n):
    
    def isValid(i, j):
        return i >= 0 and j >= 0 and i <= n and j <= i

    count = 0
    for i in xrange(n + 1):
        for j in xrange(i + 1):
            for k in xrange(1, n):
                if isValid(i, j) and isValid(i, j + k) and isValid(i + k, j) and isValid(i + k, j + 2*k) and isValid(i + 2*k, j + k) and isValid(i + 2*k, j + 2*k):
                    # print n, k
                    count += 1
    
    return count

for n in [3, 6, 20]:
    print h(n)
