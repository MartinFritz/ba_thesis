import json
import matplotlib.pyplot as plt
import numpy as np

#create benchmark class and fill it with json data
#use pyplot to print every benchmark object

with open("/Users/shiro/quicksortARM.json","r") as file:
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

counter = 0
for x in plotData:

    lol = np.arange(sizeCount)
    bar = [lol[i]*algoCount+counter*width for i in range(len(lol))]
    plt.bar(bar,[y[1] for y in plotData[x]], width, label=x, edgecolor = 'black')
    counter = counter+1

tickLocations = [i*algoCount+(width*(algoCount-1)/2) for i in range(sizeCount)]
sortedSizes = list(sizes)
sortedSizes.sort()
plt.xticks(tickLocations, labels=sortedSizes)
plt.xlabel("Algorithms grouped by element sizes")
plt.ylabel("throughput in elements/ms")
plt.legend()
plt.show()