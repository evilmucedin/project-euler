#!/usr/bin/env python3

import matplotlib
import pandas
import matplotlib.pyplot as plt

series = pandas.read_csv("optimalNZ.csv")
series['Date'] = pandas.to_datetime(series['Date'], format='%Y-%m-%d')

series.plot(x='Date')
plt.show()
