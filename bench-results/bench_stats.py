import os
import re
import sys
import pandas as pd
import numpy as np

from matplotlib import pyplot as plt
from natsort import natsorted


class Benchmark_Stat:
    def __init__(self, DIR, test_name):
        self.DIR = DIR
        self.PIC_DIR = "pic/"
        self.test_name = test_name

        self.pattern1 = "Total(\s*)\d+ Hugepage\(s\) stranded in all cpu caches."
        self.pattern2 = "HugeAllocator: \d+ requested - \d+ in use = \d+ hugepages free"

        self.totalStrandedPage = []
        self.totalHugePageRequested = []
        self.totalHugePageInUse = []
        self.totalHugePageFree = []
        self.percentage = []

        self.get_data()
        self.get_stranded_percentage()


    def get_data(self):
        for filename in natsorted(os.listdir(self.DIR)):
            if filename.endswith(".txt"): 
                with open(self.DIR + '/' + filename) as file:
                    for info in file:
                        strandedPage = re.search(self.pattern1, info)
                        if strandedPage:
                            for word in strandedPage.group().split(' '):
                                if word.isdigit():
                                    self.totalStrandedPage.append(int(word))
                                    break
                        hugePage = re.search(self.pattern2, info)
                        if hugePage:
                            for index, word in enumerate(hugePage.group().split(' ')):
                                if word.isdigit():
                                    if index == 1:
                                        self.totalHugePageRequested.append(int(word))
                                    if index == 4:
                                        self.totalHugePageInUse.append(int(word))
                                    elif index == 8:
                                        self.totalHugePageFree.append(int(word))
                            break
        self.plot()
    
    def get_stranded_percentage(self):
        if len(self.totalStrandedPage):
            for i in range(len(self.totalHugePageRequested)):
                self.percentage.append(self.totalStrandedPage[i] / self.totalHugePageRequested[i] * 100)
            self.plot_percentage()
        else:
            print("0 stranded page")

    def plot(self):
        plt.plot(self.totalStrandedPage, label="Stranded Page")
        plt.plot(self.totalHugePageRequested, label="Requested")
        plt.plot(self.totalHugePageInUse, label="In Use")
        plt.plot(self.totalHugePageFree, label="Free")
        plt.legend()
        plt.xlabel("time (s)")
        plt.ylabel("num(s) page")
        plt.title("Huge Page Usage in " + self.test_name)
        # plt.show()
        plt.savefig(self.PIC_DIR + self.test_name + '-HP-Usage.png')
    
    def plot_percentage(self):
        plt.plot(self.percentage, label="Percentage")
        plt.xlabel("time (s)")
        plt.ylabel("%")
        plt.title("% Huge Page Stranded in " + self.test_name)
        # plt.show()
        plt.savefig(self.PIC_DIR + self.test_name + '-HP-Stranded.png')


class Memory_Stat:
    def __init__(self, DIR):
        self.DIR = DIR
        self.PIC_DIR = "pic/"
        self.date = ''
        self.test_name = ''
        self.memo = ''
        self.df = None
        self.get_data()
    
    def get_data(self):
        file_list = [os.path.join(self.DIR, f) for f in os.listdir(self.DIR)]
        latest_file = max(file_list, key=os.path.getctime)

        f = open(latest_file, "r")
        self.date = f.readline()
        self.test_name = f.readline()
        self.memo = f.readline()
        columns = f.readline().split()[1:]
        data = list()

        for line in f:
            data.append(line.split()[1:])

        self.df = pd.DataFrame(data=data, columns=columns)
        self.df = self.df.astype({"RSS":np.int, "PSS":np.int, "USS":np.int, })
        self.plot()
    
    def plot(self):
        # plt.plot(df.VSS)
        plt.plot(self.df.RSS, label="RSS")
        plt.plot(self.df.PSS, label="PSS")
        plt.plot(self.df.USS, label="USS")
        plt.legend()
        plt.xlabel("time (s)")
        plt.ylabel("Mem usage")
        plt.title(self.date + " " + self.test_name + " " + self.memo)
        # plt.show()
        plt.savefig(self.PIC_DIR + self.test_name + '-Mem-Usage.png')

mem_dir = "/home/minh/Desktop/tcmalloc/bench-results/smem/"
stat_dir = "/home/minh/Desktop/tcmalloc/bench-results/stats/"
tests = ["PING_INLINE", "PING_MBULK", "INCR", "LPUSH", "RPUSH", "LPOP", "RPOP",
        "SADD","SET", "GET", "MSET", "LRANGE"]
release_rates = ["0MB"]

for test_name in tests:
    for rate in release_rates:
        current_stat_dir = stat_dir + test_name + "/" + rate
        current_mem_dir = mem_dir + test_name + "/" + rate
        test_bench_stat = Benchmark_Stat(current_stat_dir, test_name + "-" + rate)
        test_mem_stat = Memory_Stat(current_mem_dir)