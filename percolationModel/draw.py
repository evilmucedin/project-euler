#!/usr/bin/env python3
"""Draw conductivity of a metal/dielectric mixture vs. metal fraction.

Reads conductivity.csv produced by percolationModel and writes percolation.png.
"""

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np

data = np.loadtxt("conductivity.csv", delimiter=",", skiprows=1)
p, mean, std = data[:, 0], data[:, 1], data[:, 2]
jMean, jStd = data[:, 3], data[:, 4]

fig, (axLinear, axLog, axCurrent) = plt.subplots(1, 3, figsize=(18, 5))

axLinear.errorbar(p, mean, yerr=std, fmt="o-", markersize=3, capsize=2, color="tab:blue")
axLinear.axvline(0.5927, color="tab:red", linestyle="--", label="site percolation threshold 0.5927")
axLinear.set_xlabel("metal fraction p")
axLinear.set_ylabel("effective conductivity (metal = 1)")
axLinear.set_title("Conductivity of metal/dielectric mixture")
axLinear.legend()
axLinear.grid(alpha=0.3)

axLog.errorbar(p, mean, yerr=std, fmt="o-", markersize=3, capsize=2, color="tab:blue")
axLog.axvline(0.5927, color="tab:red", linestyle="--")
axLog.set_yscale("log")
axLog.set_xlabel("metal fraction p")
axLog.set_ylabel("effective conductivity (log scale)")
axLog.set_title("Same data, log scale")
axLog.grid(alpha=0.3)

axCurrent.errorbar(p, jMean, yerr=jStd, fmt="o-", markersize=3, capsize=2, color="tab:orange")
axCurrent.axvline(0.5927, color="tab:red", linestyle="--")
axCurrent.set_yscale("log")
axCurrent.set_xlabel("metal fraction p")
axCurrent.set_ylabel("average current density (log scale)")
axCurrent.set_title("Average current density in the sample")
axCurrent.grid(alpha=0.3)

fig.tight_layout()
fig.savefig("percolation.png", dpi=150)
print("wrote percolation.png")
