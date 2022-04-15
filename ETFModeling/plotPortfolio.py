#!/usr/bin/env python3

import matplotlib
import pandas
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser(description='Plot portfolio.')
parser.add_argument('--input', type=str, default='optimalNZ.csv', help='input csv')
args = parser.parse_args()

series = pandas.read_csv(args.input)
series['Date'] = pandas.to_datetime(series['Date'], format='%Y-%m-%d')

series.plot(x='Date')
plt.show()
