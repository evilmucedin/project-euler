#!/usr/bin/env python3

import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
 
for filename in ["normalMC.csv", "normalInvCDF.csv", "normalBoxMuller.csv", "normalNaive.csv", "uniform.csv"]:
    x = np.loadtxt(filename)
    num_bins = 100
    n, bins, patches = plt.hist(x, num_bins, facecolor='blue', alpha=0.5)
    plt.show()
