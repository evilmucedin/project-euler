#!/usr/bin/env python3

import pandas
import numpy as np
import matplotlib.pyplot as plt

data = pandas.read_csv("s01.txt", header=None)
print(data.head())

figure, ax1 = plt.subplots()
ax1.plot(data[0])
plt.show()

plt.clf()
plt.cla()
figure, ax1 = plt.subplots()
freq = np.fft.fft(data[0])
ax1.plot(freq.real)
figure.savefig("fft.png")
plt.show()
