#!/usr/bin/env python3

from requests import get, post
import json
import os
from multiprocessing import Pool, Value
import dummy

res = get("https://api.iextrading.com/1.0/ref-data/symbols")
jSymbols = json.loads(res.text)
# print(dumps(jSymbols, indent=4))
symbols = []
for s in jSymbols:
    symbols.append((s["symbol"]))

DIR = "dividends"

try:
    os.mkdir(DIR)
except:
    pass

def fetchSymbol(s):
    def fetch(url, filename):
        resp = get(url)
        with open(filename, "w") as fOut:
            if resp.text:
                try:
                    jResp = json.loads(resp.text)
                except json.decoder.JSONDecodeError:
                    print("fail to decode '%s'" % resp.text)
                    jResp = {}
                print("%s" % json.dumps(jResp, indent=4), file=fOut)
            else:
                print("%s failed" % s)

    fetch("https://api.iextrading.com/1.0/stock/%s/dividends/5y" % s, "%s/%s.div.json" % (DIR, s))
    fetch("https://api.iextrading.com/1.0/stock/%s/chart/5y" % s, "%s/%s.chart.json" % (DIR, s))
    dummy.v.value += 1
    print("%d/%d" % (dummy.v.value, len(symbols)))

def initProcess(v):
  dummy.v = v

v = Value('i', 0)
p = Pool(16, initializer=initProcess, initargs=(v,))
results = p.map(fetchSymbol, symbols)
