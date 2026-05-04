#!/usr/bin/env python3

import matplotlib.pyplot as plt

xs = []
ys = []
for line in open("second.out"):
    parts = line.split()
    xs.append(int(parts[0]))
    ys.append(100 - int(parts[1]))

plt.scatter(xs, ys)
plt.show()
