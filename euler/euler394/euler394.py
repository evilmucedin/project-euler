#!/usr/bin/env python
BINS = 10000000
x = 40

def interp(dist):
    newdist = [0]*BINS
    temp = 0

    for i in range(BINS - 2, BINS/x - 1, -1):
        newdist[i] = newdist[i+1] + temp
        temp += dist[i]/(i*(i+1)/2.+.25)

    for i in range(BINS - 1, BINS/x - 1, -1):
        newdist[i] += .25/(i*(i+1)/2.+.25)*dist[i]

    return newdist

esum = 1.0
dist = [((BINS-i)*2 - 1)/float(BINS**2) for i in range(BINS)]

for i in range(20):
    prob = sum(dist[BINS/x:])
    esum += prob
    print(str(i) + "\t" + str(prob))
    dist = interp(dist)
print(esum)
