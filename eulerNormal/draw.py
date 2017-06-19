#!/usr/bin/env python3

import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
 
x = np.loadtxt("uniform.csv")
num_bins = 5
n, bins, patches = plt.hist(x, num_bins, facecolor='blue', alpha=0.5)
plt.show()
