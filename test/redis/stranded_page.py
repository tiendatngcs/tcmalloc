import pandas as pd
import sys
import numpy as np
import glob
import os
import re

from matplotlib import pyplot as plt
from natsort import natsorted

DIR = "/home/minh/Desktop/redis/src/Stats"

pattern1 = "Total \d+ stranded hugepage between all cpu caches."
pattern2 = "HugeAllocator: \d+ requested - \d+ in use = \d+ hugepages free"

totalStrandedPage = []
totalHugePageRequested = []
totalHugePageInUse = []
totalHugePageFree = []

for filename in natsorted(os.listdir(DIR)):
    if filename.endswith(".txt"): 
        with open(DIR + '/' + filename) as file:
            for info in file:
                strandedPage = re.search(pattern1, info)
                if strandedPage:
                    for word in strandedPage.group().split(' '):
                        if word.isdigit():
                            totalStrandedPage.append(int(word))
                            break
                hugePage = re.search(pattern2, info)
                if hugePage:
                    for index, word in enumerate(hugePage.group().split(' ')):
                        if word.isdigit():
                            if index == 1:
                                totalHugePageRequested.append(int(word))
                            if index == 4:
                                totalHugePageInUse.append(int(word))
                            elif index == 8:
                                totalHugePageFree.append(int(word))
                    break

# print(totalStrandedPage)
# print(totalHugePageRequested)
# print(totalHugePageInUse)
# print(totalHugePageFree)

plt.plot(totalStrandedPage, label="Stranded Page")
plt.plot(totalHugePageRequested, label="Requested")
plt.plot(totalHugePageInUse, label="In Use")
plt.plot(totalHugePageFree, label="Free")
plt.legend()
plt.xlabel("time (s)")
plt.ylabel("num(s) page")
plt.title("Huge Page")
plt.show()
