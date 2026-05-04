#!/usr/bin/env python3

import pandas
import numpy as np
import matplotlib.pyplot as plt

def multiMode():
    for a in ["walking", "standing", "sitting", "running"]:
        data = pandas.read_csv("%s.txt" % a, header=None)
        print(data.head())

        plt.title(a)
        figure, ax1 = plt.subplots()
        ax1.plot(data[0])
        ax1.plot(data[1])
        ax1.plot(data[2])
        figure.savefig("%sData.png" % a)
        plt.show()

        plt.clf()
        plt.cla()
        figure, ax1 = plt.subplots()
        freq = np.fft.fft(data[0])
        ax1.plot(freq.real[1:])
        figure.savefig("%sFFT.png" % a)
        plt.show()

def arduinoMode():
    for a in ["arduino.csv"]:
        data = pandas.read_csv(a)
        print(data.head())

        plt.title(a)
        figure, ax1 = plt.subplots()
        ax1.plot(data["AxX"])
        ax1.plot(data["AxY"])
        ax1.plot(data["AxZ"])
        figure.savefig("%sData.png" % a)
        plt.show()

        plt.clf()
        plt.cla()
        figure, ax1 = plt.subplots()
        freq = np.fft.fft(data["AxZ"])
        ax1.plot(freq.real[1:])
        figure.savefig("%sFFT.png" % a)
        plt.show()

def s2(n):
    s = str(n)
    while len(s) < 2:
        s = "0" + s
    return s

def singleMode():
    figure, ax1 = plt.subplots()
    for iPerson in range(8):
        for iTrack in range(60):
            print(iPerson + 1, iTrack + 1)
            data = pandas.read_csv("data/a10/p%s/s%s.txt" % (str(iPerson + 1), s2(iTrack + 1)), header=None)
            freq = np.fft.fft(data[0])
            ax1.plot(freq.real[1:])
    figure.savefig("allRunningFFT.png")
    plt.show()

if __name__ == "__main__":
    # multiMode()
    # singleMode()
    arduinoMode()
