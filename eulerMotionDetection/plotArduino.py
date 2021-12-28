#!/usr/bin/env python3

import matplotlib.pyplot as plt
import pandas as pd

data = pd.read_csv("arduino.csv")
print(data.head())

# figure, ax1 = plt.subplots()
# ax1.plot(data["AxZ"])
data.plot(x='Timestamp', y="AxZ")
plt.show()
