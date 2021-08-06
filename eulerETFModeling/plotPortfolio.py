#!/usr/bin/env python3

import matplotlib
import pandas
import matplotlib.pyplot as plt

series = pandas.read_csv("optimal.csv")

series.plot()
plt.show()
