#!/usr/bin/env python3

import math

LIMIT = 3000

s, l = 0, []
for x1 in range(-LIMIT, LIMIT):
    for x2 in range(-LIMIT, LIMIT):
        for y1 in range(-LIMIT, LIMIT):
            k = x1**2 + y1**2 - x2**2
            if k < 0 or int(k**.5) != k**.5:
                continue
            y2 = int(k**.5)
            x3 = 5 - x1 - x2
            y3 = -y1 - y2
            d1 = (x1 - x2)**2
            d2 = (y1 - y2)**2
            d3 = (x1 - x3)**2
            d4 = (y1 - y3)**2
            d5 = (x2 - x3)**2
            d6 = (y2 - y3)**2
            a = math.sqrt(d1 + d2)
            b = math.sqrt(d3 + d4)
            c = math.sqrt(d5 + d6)
            p = a + b + c
            if p > 100000:
                break
            o1 = (x1**2) + (y1**2)
            o2 = (x2**2) + (y2**2)
            o3 = (x3**2) + (y3**2)
            if a + b > c and a + c > b and b + c > a and o1 == o2 and o1 == o3:
                tmp = sorted([(x1, y1), (x2, y2), (x3, y3)])
                if tmp not in l:
                    l.append(tmp)
                    s += p
print(s)
