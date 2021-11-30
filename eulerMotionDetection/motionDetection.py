#!/usr/bin/env python3

import pandas
import numpy as np
import matplotlib.pyplot as plt

for a in ["walking", "standing", "sitting", "running"]:
	data = pandas.read_csv("%s.txt" % a, header=None)
	print(data.head())

	figure, ax1 = plt.subplots()
	ax1.plot(data[0])
	ax1.plot(data[1])
	ax1.plot(data[2])
	figure.savefig("%sData.png" % a)
	plt.show()

	plt.clf()
	plt.cla()
	figure, ax1 = plt.subplots()
	freq = np.fft.fft(data[0])
	ax1.plot(freq.real[1:])
	figure.savefig("%sFFT.png" % a)
	plt.show()
