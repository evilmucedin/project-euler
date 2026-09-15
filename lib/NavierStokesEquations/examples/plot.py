#!/usr/bin/env python3
"""Plots for the Navier-Stokes examples.

Each example writes plain CSV next to itself; run this script from the same
directory afterwards:

    ./build/ninja/bin/lib/NavierStokesEquations/examples/lidDrivenCavity
    python3 lib/NavierStokesEquations/examples/plot.py cavity

Needs numpy and matplotlib. The PPM images the examples write need neither and
can be viewed directly, or turned into a movie with

    ffmpeg -framerate 10 -i cylinderWake%04d.ppm cylinderWake.mp4
"""

import sys

import matplotlib.pyplot as plt
import numpy as np


def readColumns(path):
    """Reads a writeColumnsCsv file into a dict of column name to array.

    Columns may be shorter than the file has rows, so the trailing blanks are
    dropped rather than read as zeros.
    """
    with open(path) as handle:
        headers = handle.readline().strip().split(",")
        rows = [line.strip().split(",") for line in handle if line.strip()]
    columns = {}
    for index, name in enumerate(headers):
        values = [row[index] for row in rows if index < len(row) and row[index] != ""]
        columns[name] = np.array([float(value) for value in values])
    return columns


def readField(path):
    """Reads a writeFieldCsv file. The result is indexed [j][i] with y ascending."""
    return np.loadtxt(path, delimiter=",")


def plotTaylorGreen():
    columns = readColumns("taylorGreenConvergence.csv")
    n, rms, maximum = columns["n"], columns["rms"], columns["max"]

    figure, axes = plt.subplots(figsize=(6, 4.5))
    axes.loglog(n, rms, "o-", label="rms error")
    axes.loglog(n, maximum, "s-", label="max error")
    axes.loglog(n, rms[0] * (n[0] / n) ** 2, "k--", label="second order")
    axes.set_xlabel("cells per side")
    axes.set_ylabel("velocity error")
    axes.set_title("Taylor-Green vortex, grid convergence")
    axes.grid(True, which="both", alpha=0.3)
    axes.legend()
    figure.tight_layout()
    figure.savefig("taylorGreen.png", dpi=150)
    print("wrote taylorGreen.png")


def plotCavity():
    columns = readColumns("cavityCentrelines.csv")
    vorticity = readField("cavityVorticity.csv")
    velocity = np.loadtxt("cavityVelocity.csv", delimiter=",", skiprows=1)

    figure, axes = plt.subplots(1, 3, figsize=(15, 4.5))
    axes[0].plot(columns["uCenterline"], columns["y"], "-")
    axes[0].axvline(0.0, color="k", lw=0.5)
    axes[0].set_xlabel("u")
    axes[0].set_ylabel("y")
    axes[0].set_title("u on the vertical centreline")
    axes[0].grid(alpha=0.3)

    axes[1].plot(columns["x"], columns["vCenterline"], "-")
    axes[1].axhline(0.0, color="k", lw=0.5)
    axes[1].set_xlabel("x")
    axes[1].set_ylabel("v")
    axes[1].set_title("v on the horizontal centreline")
    axes[1].grid(alpha=0.3)

    limit = np.percentile(np.abs(vorticity), 98)
    axes[2].imshow(vorticity, origin="lower", extent=(0, 1, 0, 1), cmap="RdBu_r",
                   vmin=-limit, vmax=limit)
    # The velocity file is one row per cell, x fastest.
    side = int(round(np.sqrt(velocity.shape[0])))
    x = velocity[:, 0].reshape(side, side)
    y = velocity[:, 1].reshape(side, side)
    u = velocity[:, 2].reshape(side, side)
    v = velocity[:, 3].reshape(side, side)
    step = max(1, side // 20)
    axes[2].quiver(x[::step, ::step], y[::step, ::step], u[::step, ::step], v[::step, ::step],
                   scale=8, width=0.004)
    axes[2].set_title("vorticity and velocity")
    axes[2].set_xlabel("x")
    axes[2].set_ylabel("y")

    figure.tight_layout()
    figure.savefig("lidDrivenCavity.png", dpi=150)
    print("wrote lidDrivenCavity.png")


def plotChannel():
    columns = readColumns("channelProfile.csv")

    figure, axes = plt.subplots(1, 2, figsize=(10, 4.5))
    axes[0].plot(columns["computed"], columns["y"], "o", ms=4, label="computed")
    axes[0].plot(columns["exact"], columns["y"], "-", label="G y (h - y) / 2 nu")
    axes[0].set_xlabel("u")
    axes[0].set_ylabel("y")
    axes[0].set_title("Poiseuille profile")
    axes[0].grid(alpha=0.3)
    axes[0].legend()

    n, error = columns["ny"], columns["maxError"]
    axes[1].loglog(n, error, "o-", label="max error")
    axes[1].loglog(n, error[0] * (n[0] / n) ** 2, "k--", label="second order")
    axes[1].set_xlabel("cells across the channel")
    axes[1].set_ylabel("max error")
    axes[1].set_title("grid convergence")
    axes[1].grid(True, which="both", alpha=0.3)
    axes[1].legend()

    figure.tight_layout()
    figure.savefig("channelFlow.png", dpi=150)
    print("wrote channelFlow.png")


def plotCylinder():
    columns = readColumns("cylinderProbe.csv")
    time, v = columns["time"], columns["v"]

    figure, axes = plt.subplots(1, 2, figsize=(11, 4))
    axes[0].plot(time, v, lw=0.8)
    axes[0].set_xlabel("time")
    axes[0].set_ylabel("v at the probe")
    axes[0].set_title("wake oscillation")
    axes[0].grid(alpha=0.3)

    # Spectrum of the second half, where the shedding is established.
    late = v[time >= 0.5 * time[-1]]
    lateTime = time[time >= 0.5 * time[-1]]
    if len(late) > 8:
        dt = (lateTime[-1] - lateTime[0]) / (len(lateTime) - 1)
        spectrum = np.abs(np.fft.rfft(late - late.mean()))
        frequency = np.fft.rfftfreq(len(late), dt)
        axes[1].semilogy(frequency, spectrum, lw=0.8)
        peak = frequency[np.argmax(spectrum)]
        axes[1].axvline(peak, color="r", ls="--", label=f"f = {peak:.3f}, St = {0.2 * peak:.3f}")
        axes[1].set_xlim(0, 10 * peak if peak > 0 else 5)
        axes[1].legend()
    axes[1].set_xlabel("frequency")
    axes[1].set_ylabel("amplitude")
    axes[1].set_title("shedding spectrum")
    axes[1].grid(alpha=0.3)

    figure.tight_layout()
    figure.savefig("cylinderWake.png", dpi=150)
    print("wrote cylinderWake.png")


PLOTS = {
    "taylorGreen": plotTaylorGreen,
    "cavity": plotCavity,
    "channel": plotChannel,
    "cylinder": plotCylinder,
}


def main(argv):
    names = argv[1:] or list(PLOTS)
    unknown = [name for name in names if name not in PLOTS]
    if unknown:
        print(f"unknown plot(s): {', '.join(unknown)}", file=sys.stderr)
        print(f"available: {', '.join(PLOTS)}", file=sys.stderr)
        return 2
    for name in names:
        PLOTS[name]()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
