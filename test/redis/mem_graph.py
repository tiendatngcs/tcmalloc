from matplotlib import pyplot as plt
import pandas as pd
import sys
import numpy as np


f = open("./smem/malloc_profile_061821_122558.txt", "r")
# date = f.readline()
test_name = f.readline()
columns = f.readline().split()[1:]
data = list()

for line in f:
    data.append(line.split()[1:])

df = pd.DataFrame(data=data, columns=columns)
df = df.astype({"RSS":np.int, "PSS":np.int, "USS":np.int, })


# plt.plot(df.VSS)
plt.plot(df.RSS, label="RSS")
plt.plot(df.PSS, label="PSS")
plt.plot(df.USS, label="USS")
plt.legend()
plt.xlabel("time (s)")
plt.ylabel("Mem usage")
plt.title(test_name)

plt.show()
