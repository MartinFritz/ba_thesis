import json
import matplotlib.pyplot as plt
import numpy as np

import matplotlib
from math import sqrt






MARKERLIST = ['o','x','d','+','^','*', 's']
with open("/Users/shiro/hashtable_benchmark_laptop.json","r") as file:
    data = json.load(file)
labels = []
cputime = []
samplesize = []
plotData = dict()
algs = set()
sizes = set()
for x in data["benchmarks"]:
    if '/' in x["name"]:
        # labels.append(x["name"].split("/")[0])
        # cputime.append(float(x["cpu_time"]))
        # samplesize.append(int(x["name"].split("/")[1]))
        sampleS = int(x["name"].split("/")[1])
        name = x["name"].split("/")[0]
        cpuT = float(x["cpu_time"])
        algs.add(name)
        sizes.add(sampleS)
        if name in plotData:
            plotData[name].append([sampleS,sampleS/cpuT])
        else:
            plotData[name] = []
            plotData[name].append([sampleS,sampleS/cpuT])
print(plotData)
algoCount = len(algs)
sizeCount = len(sizes)

width = 0.6
xlist = []
ylist = []
labels = []
counter = 0
for x in plotData:
    xlist.append([])
    ylist.append([])
    labels.append(x)
    for y in plotData[x]:
        xlist[len(xlist)-1].append("{:.0e}".format(y[0]))
        ylist[len(ylist)-1].append(y[1])

font = {'family' : 'serif',
        'size'   : 10}

matplotlib.rc('font', **font)
for i in range(0,len(xlist)):
    plt.plot(xlist[i],ylist[i],marker=MARKERLIST[i], label=labels[i])
plt.xlabel('element size')
plt.ylabel('throughput in elements/ms')
plt.tight_layout()
plt.legend()

plt.savefig("hashing_laptop.pdf")

plt.show()