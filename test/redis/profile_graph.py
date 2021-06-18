from matplotlib import pyplot as plt
import pandas as pd
import sys
import numpy as np
import argparse

argparser = argparse.ArgumentParser()
argparser.add_argument('A', metavar='Memory or Swap Space', type=str, nargs=1,
                    help='[mem, swap]')
argparser.add_argument('B', metavar='Section to graph', type=str, nargs=1,
                    help='[total, used, free, shared, buff/cache, available]')

argvs = argparser.parse_args()
# if len(argvs) != 3:
#     print("Program needs 2 argument values: a[mem, swap], b[total, used, free, shared, buff/cache, available]")
#     exit(1)
a, b = argvs.A[0], argvs.B[0]
f = open("malloc_profile.txt", "r")
date = f.readline()
f.readline()
mem_titles = f.readline().split()
swap_titles = mem_titles[:3]
mems = list()
swaps = list()
for line in f:
    words = line.split()
    if words[0] == "Mem:":
        words.pop(0)
        mems.append(words)
    elif words[0] == "Swap:":
        words.pop(0)
        swaps.append(words)
    else:
        pass
# print(mem_titles)

mems_df = pd.DataFrame(data=mems, columns=mem_titles, dtype=np.float)
swaps_df = pd.DataFrame(data=swaps, columns=swap_titles, dtype=np.float)
df_dict = {"mem": mems_df, "swap": swaps_df}

y = df_dict[a][b].to_numpy()
x = list(range(len(y)))
plt.plot(x, y, color='red', marker='o')
plt.xticks(np.arange(min(x), max(x), 5))
plt.yticks(np.arange(min(y), max(y), 10000))
plt.xlabel("Time (s)")
plt.ylabel(a + " " + b)
plt.title("Memory profile over time")
plt.show()
