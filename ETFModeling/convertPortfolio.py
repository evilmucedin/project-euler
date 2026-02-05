#!/usr/bin/env python3

import sys
import pandas as pd
import numpy as np

pdData = pd.read_csv(sys.argv[1])
pdData = pdData[pdData['Account Name'].notna()]
pdData['Current Value'] = pdData['Current Value'].replace('[\$,]', '', regex=True).astype(float)
total = float(pdData['Current Value'].sum())
print(total)
pdData['Current Value'] = pdData['Current Value'].div(total)
print(pdData)
# pdExport = pdData[['Symbol', 'Current Value']]
pdExport = pdData[['Account Name', 'Current Value']]
print(pdExport)
# pdExport = pdExport[pdExport['Symbol'] != 'CORE**']
pdExport.to_csv('portfolio.csv', index=False)
