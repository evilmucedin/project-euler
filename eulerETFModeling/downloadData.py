#!/usr/bin/env python3

# from inspect import getmembers, isfunction

# Import the yfinance. If you get module not found error the run !pip install yfinance from your Jupyter notebook
import yfinance

# print(help(yfinance))

tickers = ["FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG", "MSFT", "T", "NCLH", "OGZPY", "SPY", "IVV", "VOO", "QQQ"]

for ticker in tickers:
    data = yfinance.download(ticker, '2010-08-01', '2021-08-01')
    data.to_csv("%s.csv" % ticker)

# Import the plotting library
import matplotlib.pyplot as plt
# %matplotlib inline

# Plot the close price of the AAPL
data['Adj Close'].plot()
plt.show()
