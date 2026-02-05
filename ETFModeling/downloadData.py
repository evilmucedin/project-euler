#!/usr/bin/env python3

# from inspect import getmembers, isfunction

# Import the yfinance. If you get module not found error the run !pip install yfinance from your Jupyter notebook
import matplotlib.pyplot as plt
import yfinance
from datetime import date, timedelta

#   print(help(yfinance))

tickers = ["FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "GOOG", "IWM", "VUG", "MSFT", "T", "NCLH", "IBM",
           "SPY", "IVV", "VOO", "QQQ", "AMZN", "META", "TSLA", "BND", "FBND", "HDV", "VEU", "VWO", "FDHY", "FDIS", "ONEQ", "VV", "VB", "HNDL", "WBII", "PCEF", "FDIV", "CEFS", "YLD", "INKM", "IYLD", "FCEF", "MLTI", "YYY", "MDIV", "HIPS", "CVY", "GYLD", "VTI", "VEA", "IEFA", "AGG", "GLD", "XLF", "VNQ", "LQD", "SWPPX", "MGK", "GME", "UNG", "OIH", "XME", "PFIX", "VXX", "EWZ", "ILF", "SCHE",
           "AAPL", "NVDA", "TSM", "UNH", "JNJ", "V", "WMT", "JPM", "PG", "XOM", "HD", "CVX", "PFE", "VWAGY", "KHC", "NKE", "SBUX",
           "FBCG", "FQAL", "FLPSX", "FDRR", "FMAG", "FPRO", "FBCV", "FMIL",
           "COIN", "LMT", "FDIG", "ARKK", "FELG", "FTEC", "FDVV",
           "BITO", "BITW", "VBB", "SFY", "IJR", "SCHD", "FTLS", "FDHT", "FRNW", "FDRV", "FCLD", "ITA", "PPA", "XAR", "USO", "IEO", "SPGP", "IWY", "SPYG",
           "BAMXF"
           # "59101K877", "09259A395"
           ]
assert(len(tickers) == len(set(tickers)))

for ticker in tickers:
    data = yfinance.download(ticker, start='2000-08-01', end=str(date.today() - timedelta(days=1)), actions=True, auto_adjust=False)
    filename = "marketData/%s.csv" % ticker
    data.to_csv(filename)
    if True:
        with open(filename, "r") as f:
            if True:
                lines = f.readlines()
                print(ticker, len(lines), filename, lines[2], lines[0])
                f.close()
                lines[0] = lines[2][:5] + lines[0][6:]
                lines.pop(1)
                lines.pop(1)
                with open(filename + "", "w") as w:
                    w.writelines(lines)

# Import the plotting library
# %matplotlib inline

# Plot the close price of the AAPL
data['Adj Close'].plot()
plt.show()
