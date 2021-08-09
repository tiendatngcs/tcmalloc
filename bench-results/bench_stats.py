import os
import re
import sys
import pandas as pd
import numpy as np

from matplotlib import pyplot as plt
from natsort import natsorted


class Log:
    def __init__(self, DIR, test_name):
        self.LOG_DIR = os.path.join(DIR, "log/")
        self.file_name = self.LOG_DIR + test_name + ".txt"
        self.deallocate_time = set()
        self.offset = 0
    
    def convert_to_sec(self, time_string):
        ftr = [3600, 60, 1]
        sec_string = sum([a * b for a, b in zip(ftr, map(int, time_string.split(':')))])
        return sec_string

    def get_log(self):
        f = open(self.file_name)
        current_time = ""
        
        for index, line in enumerate(f):
            if not index:
                continue
            # ['15:38:56', '', 'Round 0: Running redis SET: ./redis/bench.sh SET 8 10366']
            line = line.strip().split("\t")

            # get the offset
            if not self.offset:
                self.offset = self.convert_to_sec(line[0])
            
            # there could be multiple deallocation at 1 time step
            if line[0] == current_time:
                continue

            # check if it is a deallocate
            if line[2][0] == '.':
                current_time = self.convert_to_sec(line[0]) - self.offset
                self.deallocate_time.add(current_time)
            current_time = line[0]
        return sorted(self.deallocate_time)


class Benchmark_Stat:
    def __init__(self, DIR, test_name, deallocate_log=None):
        self.DIR = DIR
        self.PIC_DIR = os.path.join("/home/minh/Desktop/tcmalloc/bench-results/", "pic/")
        self.test_name = test_name

        self.pattern1 = "Total(\s*)\d+ Hugepage\(s\) stranded in all cpu caches."
        self.pattern2 = "HugeAllocator: \d+ requested - \d+ in use = \d+ hugepages free"

        self.totalStrandedPage = []
        self.totalHugePageRequested = []
        self.totalHugePageInUse = []
        self.totalHugePageFree = []
        self.percentage = []
        
        self.deallocate_log = deallocate_log

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
        if self.deallocate_log:
            for coor in self.deallocate_log:
                plt.axvline(x=coor, c="skyblue", ls='--')
        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.savefig(os.path.join(self.PIC_DIR, self.test_name + '-HP-Usage.png'), dpi = 100)
        plt.show()
    
    def plot_percentage(self):
        textstr = '\n'.join((f"Max Requested HP {max(self.totalHugePageRequested)}",
                            f"Min Requested HP {min(self.totalHugePageRequested)}",
                            f"Max Stranded {max(self.totalStrandedPage)}",
                            f"Min Stranded {min(self.totalStrandedPage)}"))
        plt.plot(self.percentage, label="Percentage")
        plt.xlabel("time (s)")
        plt.ylabel("%")
        plt.title("% Huge Page Stranded in " + self.test_name)
        if self.deallocate_log:
            for coor in self.deallocate_log:
                plt.axvline(x=coor, c="skyblue", ls='--')
        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.text(0.8, 0.9, textstr, fontsize = 12, bbox = dict(facecolor = 'white', alpha = 0.5))
        fig.savefig(os.path.join(self.PIC_DIR, self.test_name + '-HP-Stranded.png'), dpi = 100)
        plt.show()


class Memory_Stat:
    def __init__(self, DIR, test_name, deallocate_log=None):
        self.DIR = DIR
        self.PIC_DIR = os.path.join("/home/minh/Desktop/tcmalloc/bench-results/", "pic/")

        self.pic_name = test_name
        self.deallocate_log = deallocate_log

        self.date = ''
        self.test_name = ''
        self.memo = ''
        self.df = None
        self.get_data()
    
    def get_data(self):
        file_list = [os.path.join(self.DIR, f) for f in os.listdir(self.DIR)]
        latest_file = max(file_list, key=os.path.getctime)

        f = open(latest_file, "r")
        self.test_name = f.readline()
        self.memo = f.readline()
        self.date = f.readline()
        columns = f.readline().split()[2:]
        data = list()

        for line in f:
            data.append(int(x) / 1000 for x in line.split()[2:])

        self.df = pd.DataFrame(data=data, columns=columns)
        self.df = self.df.astype({"RSS":np.int, "PSS":np.int, "USS":np.int})
        self.plot()
    
    def plot(self):
        # plt.plot(df.VSS)
        plt.plot(self.df.RSS, label="RSS")
        plt.plot(self.df.PSS, label="PSS")
        plt.plot(self.df.USS, label="USS")
        plt.legend()
        plt.xlabel("time (s)")
        plt.ylabel("Mem usage (MB)")
        plt.title(self.date + " " + self.test_name + " " + self.memo)
        if self.deallocate_log:
            for coor in self.deallocate_log:
                plt.axvline(x=coor, c="skyblue", ls='--')
        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.savefig(os.path.join(self.PIC_DIR, self.pic_name  + '-Mem-Usage.png'), dpi = 100)
        plt.show()


class Driver:
    def __init__(self, test_suite, tests, release_rates):
        self.redis_smem_dir = "/home/minh/Desktop/tcmalloc/bench-results/smem/"
        self.redis_stat_dir = "/home/minh/Desktop/tcmalloc/bench-results/stats/"
        self.redis_log_dir = "/home/minh/Desktop/tcmalloc/bench-results/"

        self.mybench_smem_dir = "/home/minh/Desktop/tcmalloc/mybench/smem/"
        self.mybench_stat_dir = "/home/minh/Desktop/tcmalloc/mybench/stats/"
        self.mybench_log_dir = "/home/minh/Desktop/tcmalloc/mybench/"

        self.tests = tests
        self.release_rates = release_rates
        self.deallocate_log = None

        if test_suite == "redis":
            self.run_redis()
        elif test_suite == "mybench":
            self.run_mybench()
    
    def run_redis(self):
        for test_name in self.tests:
            for rate in self.release_rates:
                current_stat_dir = self.redis_stat_dir + test_name + "/" + rate
                current_smem_dir = self.redis_smem_dir + test_name + "/" + rate
                deallocate_log = Log(self.redis_log_dir, test_name + "-" + rate).get_log()
                Benchmark_Stat(current_stat_dir, test_name + "-" + rate, deallocate_log)
                Memory_Stat(current_smem_dir, test_name + "-" + rate, deallocate_log)
    
    def run_mybench(self):
        for test_name in self.tests:
            for rate in self.release_rates: 
                current_stat_dir = self.mybench_stat_dir + test_name + "/" + rate
                current_smem_dir = self.mybench_smem_dir + test_name + "/" + rate
                deallocate_log = Log(self.mybench_log_dir, test_name + "-" + rate).get_log()
                Benchmark_Stat(current_stat_dir, test_name + "-" + rate, deallocate_log)
                Memory_Stat(current_smem_dir, test_name + "-" + rate, deallocate_log)


Driver("redis", ["SET"], ["0MB"])
# Driver("mybench", ["mybench"], ["0MB"])

    