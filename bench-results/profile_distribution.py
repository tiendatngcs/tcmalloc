import os
import matplotlib.pyplot as plt
import numpy as np

x = []
y = []

f = open("distribution.txt", "r")
for index, line in enumerate(f):
    if index > 99:
        break
    line = line.strip().split(" ")
    x.append(float(line[0]))
    y.append(float(line[1]))

plt.plot(x, y)
plt.xticks(np.arange(1, 100, 2.0))
plt.xlabel("size (b)")
plt.ylabel("rate")
plt.legend()
plt.show()