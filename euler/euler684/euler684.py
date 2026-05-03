#!/usr/bin/env python3

def digitsSum(n):
    result = 0
    while n != 0:
        result += n % 10
        n //= 10
    return result

def s(n):
    res = n % 9
    while n >= 9:
        res = 10*res + 9
        n -= 9
    return res

def ss(n):
    res = 0
    for i in range(1, n + 1):
        res += s(i)
    return res

MOD = 1000000007

# for i in range(1, 400000):
#     print(s(i), s(i) % MOD)

fprevprev = 0
fprev = 1
for i in range(2, 91):
    f = fprev + fprevprev
    fprevprev = fprev
    fprev = f
    print(i, fprev)
