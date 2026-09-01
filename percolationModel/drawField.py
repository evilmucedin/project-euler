#!/usr/bin/env python3
"""Visualize percolation samples: material grid, potential, current density.

Generate the data with the "field" mode of percolationModel, e.g.:

  ../build-ninja/bin/percolationModel/percolationModel field 48 0.50 1 field_p50
  ../build-ninja/bin/percolationModel/percolationModel field 48 0.59 1 field_p59
  ../build-ninja/bin/percolationModel/percolationModel field 48 0.70 1 field_p70

then run:

  python3 drawField.py field_p50 field_p59 field_p70

Writes percolationField.png with one column per sample: the random
metal/dielectric grid, the solved potential field, and the current density
(log scale) that highlights the percolating cluster carrying the current.
"""

import sys

import matplotlib

matplotlib.use("Agg")

import matplotlib.pyplot as plt
import numpy as np
from matplotlib.colors import LogNorm

prefixes = sys.argv[1:] or ["field_p50", "field_p59", "field_p70"]

fig, axes = plt.subplots(3, len(prefixes), figsize=(4 * len(prefixes), 11.5), squeeze=False)

for j, prefix in enumerate(prefixes):
    sigma = np.loadtxt(prefix + "_grid.csv", delimiter=",")
    v = np.loadtxt(prefix + "_potential.csv", delimiter=",")
    metalFraction = float(np.mean(sigma > 0.5))

    # Bond conductances (series combination of half-cells) and bond currents.
    gh = 2.0 * sigma[:, :-1] * sigma[:, 1:] / (sigma[:, :-1] + sigma[:, 1:])
    gv = 2.0 * sigma[:-1, :] * sigma[1:, :] / (sigma[:-1, :] + sigma[1:, :])
    ih = np.abs(gh * (v[:, :-1] - v[:, 1:]))
    iv = np.abs(gv * (v[:-1, :] - v[1:, :]))
    current = np.zeros_like(sigma)
    current[:, :-1] += ih / 2.0
    current[:, 1:] += ih / 2.0
    current[:-1, :] += iv / 2.0
    current[1:, :] += iv / 2.0

    ax = axes[0][j]
    ax.imshow(sigma > 0.5, cmap="gray_r", interpolation="nearest")
    ax.set_title("material, metal fraction %.2f\n(black = metal)" % metalFraction)
    ax.set_xticks([])
    ax.set_yticks([])

    ax = axes[1][j]
    im = ax.imshow(v, cmap="viridis", vmin=0.0, vmax=1.0, interpolation="nearest")
    ax.set_title("potential (top = 1V, bottom = 0V)")
    ax.set_xticks([])
    ax.set_yticks([])
    fig.colorbar(im, ax=ax, fraction=0.046)

    ax = axes[2][j]
    vmax = current.max()
    im = ax.imshow(current, cmap="inferno", norm=LogNorm(vmin=vmax * 1e-8, vmax=vmax), interpolation="nearest")
    ax.set_title("current density (log scale)")
    ax.set_xticks([])
    ax.set_yticks([])
    fig.colorbar(im, ax=ax, fraction=0.046)

fig.tight_layout()
fig.savefig("percolationField.png", dpi=150)
print("wrote percolationField.png")
