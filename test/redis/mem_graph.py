import pandas as pd
import sys
import numpy as np
import glob
import os

from matplotlib import pyplot as plt

DIR = "/home/minh/Desktop/tcmalloc/test/redis/smem"
file_list = [os.path.join(DIR, f) for f in os.listdir(DIR)]
latest_file = max(file_list, key=os.path.getctime)
print(latest_file)

f = open(latest_file, "r")
date = f.readline()
test_name = f.readline()
memo = f.readline()
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
plt.title(date + " " + test_name + " " + memo)

plt.show()
