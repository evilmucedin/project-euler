#!/usr/bin/env python3

N = 10000
minSum = [-1]*N

def digitsSum(n):
    result = 0
    while n != 0:
        result += n % 10
        n //= 10
    return result

for i in range(1, 10*N):
    ds = digitsSum(i)
    if ds < N:
        if minSum[ds] == -1:
            minSum[ds] = i

print(minSum[0:50])
