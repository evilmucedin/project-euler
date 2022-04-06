#!/usr/bin/env python3

# from inspect import getmembers, isfunction

# Import the yfinance. If you get module not found error the run !pip install yfinance from your Jupyter notebook
import matplotlib.pyplot as plt
import yfinance

# print(help(yfinance))

tickers = ["FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG", "MSFT", "T", "NCLH", "OGZPY",
           "SPY", "IVV", "VOO", "QQQ", "AMZN", "FB", "TSLA", "BND", "FBND", "HDV", "VEU", "VWO", "FDHY", "FDIS", "ONEQ", "VV", "VB", "HNDL", "WBII", "PCEF", "FDIV", "CEFS", "YLD", "INKM", "IYLD", "FCEF", "MLTI", "YYY", "MDIV", "HIPS", "CVY", "GYLD", "VTI", "VEA", "IEFA", "AGG", "GLD", "XLF", "VNQ", "LQD", "SWPPX"]
assert(len(tickers) == len(set(tickers)))

for ticker in tickers:
    data = yfinance.download(ticker, '2000-08-01', '2022-03-25')
    data.to_csv(f"{ticker}.csv")

# Import the plotting library
# %matplotlib inline

# Plot the close price of the AAPL
data['Adj Close'].plot()
plt.show()
