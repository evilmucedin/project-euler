#!/usr/bin/env python3

# from inspect import getmembers, isfunction

# Import the yfinance. If you get module not found error the run !pip install yfinance from your Jupyter notebook
import os
import matplotlib.pyplot as plt
import yfinance
from concurrent.futures import ThreadPoolExecutor
from datetime import date, timedelta

def downloadTicker(ticker):
    filename = "marketData/%s.csv" % ticker
    if os.path.exists(filename):
        os.remove(filename)
    filenameTemp = filename + ".temp"
    if os.path.exists(filenameTemp):
        os.remove(filenameTemp)
    data = yfinance.download(ticker, start='2000-08-01', end=str(date.today() - timedelta(days=1)), actions=True, auto_adjust=False)
    if data is None or data.empty:
        raise ValueError(f"Download returned no data for ticker {ticker!r}")
    data.to_csv(filenameTemp)
    with open(filenameTemp, "r") as f:
        lines = f.readlines()
        if len(lines) < 3:
            raise ValueError(f"CSV for {ticker!r} has too few lines: {filename}")
        print(ticker, len(lines), filename, lines[2], lines[0])
        lines[0] = lines[2][:5] + lines[0][6:]
        lines.pop(1)
        lines.pop(1)
        with open(filenameTemp, "w") as w:
            w.writelines(lines)
    os.rename(filenameTemp, filename)
    return data

#   print(help(yfinance))

tickers = ["FBIOX", "FNCMX", "FSEAX", "FSKAX", "FSPSX", "FXAIX", "SHOP", "GOOG", "GOOGL", "IWM", "VUG", "MSFT", "T", "NCLH", "IBM",
           "SPY", "IVV", "VOO", "QQQ", "AMZN", "META", "AVGO", "AMD", "PLTR", "TSLA", "BND", "FBND", "HDV", "VEU", "VWO", "FDHY", "FDIS", "ONEQ", "VV", "VB", "HNDL", "WBII", "PCEF", "FDIV", "CEFS", "YLD", "INKM", "IYLD", "FCEF", "MLTI", "YYY", "MDIV", "HIPS", "CVY", "GYLD", "VTI", "VEA", "IEFA", "AGG", "GLD", "XLF", "VNQ", "LQD", "SWPPX", "MGK", "GME", "UNG", "OIH", "XME", "PFIX", "VXX", "EWZ", "ILF", "SCHE",
           "AAPL", "NVDA", "TSM", "UNH", "JNJ", "V", "WMT", "JPM", "PG", "XOM", "HD", "CVX", "PFE", "VWAGY", "KHC", "NKE", "SBUX", "NBIS",
           "FBCG", "FQAL", "FLPSX", "FDRR", "FMAG", "FPRO", "FBCV", "FMIL",
           "COIN", "LMT", "FDIG", "ARKK", "FELG", "FTEC", "FDVV",
           "BITO", "BITW", "VBB", "SFY", "IJR", "SCHD", "FTLS", "FDHT", "FRNW", "FDRV", "FCLD", "ITA", "PPA", "XAR", "USO", "IEO", "SPGP", "IWY", "SPYG", "DBO", "AIQ",
           "BAMXF"
           # "59101K877", "09259A395"
           ]
assert(len(tickers) == len(set(tickers)))

with ThreadPoolExecutor(max_workers=1) as executor:
    list(executor.map(downloadTicker, tickers))
    executor.shutdown(wait=True)

# Import the plotting library
# %matplotlib inline

# Plot the close price of the AAPL
data = downloadTicker("qqq")
data['Adj Close'].plot()
plt.show()
