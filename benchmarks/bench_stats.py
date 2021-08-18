import os
import re
import sys
import pandas as pd
import numpy as np

from matplotlib import pyplot as plt
from natsort import natsorted


class Log:
    def __init__(self, DIR, test_name):
        self.LOG_DIR = DIR
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



class Memory_Stat:
    def __init__(self, DIR, PIC_DIR, test_name, deallocate_log=None):
        self.DIR = DIR
        self.PIC_DIR = PIC_DIR
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
        self.df = self.df.dropna()
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


class GraphHugePageStats:
    def __init__(self, bench_directory, test_suite, test, release_rates, profile, drain_check_cycle):
        self.dict_list = None
        self.read_files(os.path.join(bench_directory, "stats_storage", profile_name, test, release_rates, drain_check_cycle))
        # self.deallocate_log = log
        self.PIC_DIR = os.path.join(bench_directory, "pic")
        self.pic_name = profile
        self.test_name = test

    def read_files(self, stats_directory):
        print(stats_directory)
        dict_list = list()
        totalStrandedPage = list()
        totalHugePageRequested = list()
        totalHugePageInUse = list()
        totalHugePageFree = list()
        percentage = list()

        file_list = [os.path.join(stats_directory, f) for f in os.listdir(stats_directory)]
        # print(file_list)
        file_list.sort()
        file_list.sort(key=len)
        # Process files
        for file in file_list:
            hp_dict = dict()
            f = open(file, "r")
            read = f.read()

            # Process per-hugepage stats
            per_hugepage_pattern = "HugePage at addr .+\n\tLive size:\s*\d+\sbytes\n\tCPU Cache Idle size:\s*\d+\sbytes\n\tCentral Cache Idle size:\s*\d+\sbytes\n\tFree size:\s*\d+\sbytes"
            hp_text_list = re.findall(per_hugepage_pattern, read)
            for hp_text in hp_text_list:
                hp_text_lines = hp_text.split("\n")
                addr = hp_text_lines[0].split()[-1]
                live = int(hp_text_lines[1].split()[-2])
                cpu_cache_idle = int(hp_text_lines[2].split()[-2])
                # hp_dict[addr] = (live, cpu_cache_idle)
                if live != 0 or cpu_cache_idle != 0:
                    hp_dict[addr] = (live, cpu_cache_idle)
            dict_list.append(hp_dict)

            # Process stranded page stats
            num_hp_stranded_pattern = "Total\s*\d+ Hugepage\(s\) stranded in all cpu caches."
            text = re.findall(num_hp_stranded_pattern, read)[0]
            print(text)
            num_stranded_page = int(text.split()[1])
            totalStrandedPage.append(num_stranded_page)

            hp_usage_pattern = "HugeAllocator: \d+ requested - \d+ in use = \d+ hugepages free"
            text = re.findall(hp_usage_pattern, read)[0]
            print(text)
            num_requested_page = int(text.split()[1])
            num_inuse_page = int(text.split()[4])
            num_free_page = int(text.split()[8])

            totalHugePageRequested.append(num_requested_page)
            totalHugePageInUse.append(num_inuse_page)
            totalHugePageFree.append(num_free_page)

            percentage.append(num_stranded_page / num_requested_page * 100)



        self.dict_list = dict_list
        self.totalStrandedPage = totalStrandedPage
        self.totalHugePageRequested = totalHugePageRequested
        self.totalHugePageInUse = totalHugePageInUse
        self.totalHugePageFree = totalHugePageFree
        self.percentage = percentage
    
    def graph_histogram(self, timestamp=0):
        uselessness_list = [(cpu+1)/(live+1) for (live, cpu) in self.dict_list[timestamp].values() if (cpu+1)/(live+1) != 0]
        # uselessness_list = [(cpu+1)/(live+1) for (live, cpu) in self.dict_list[timestamp].values()]
        if 0 in uselessness_list: print("There is 0")
        n, bins, patches = plt.hist(x=uselessness_list, bins=100, color='#0504aa')
        plt.grid(axis='y', alpha=0.75)
        plt.xlabel('CPU Cache Idle : Live')
        plt.ylabel('Frequency')
        plt.title('Histogram at time stamp' + str(timestamp))
        # maxfreq = n.max()
        # Set a clean upper y-axis limit.
        # plt.ylim(ymax=np.ceil(maxfreq / 10) * 10 if maxfreq % 10 else maxfreq + 10)
        plt.show()

    def graph_mean_std_timeseries(self):
        x = list(range(len(self.dict_list)))
        means = list()
        stds = list()
        for hp_dict in self.dict_list:
            uselessness_list = [(cpu+1)/(live+1) for (live, cpu) in hp_dict.values()]
            means.append(np.mean(uselessness_list))
            stds.append(np.std(uselessness_list))
        # if self.deallocate_log:
        #     for coor in self.deallocate_log:
        #         plt.axvline(x=coor, c="skyblue", ls='--')
        #         pass
        plt.errorbar(x=x, y=means, yerr=stds, ecolor="r")
        print("Mean of means", np.mean(means))
        plt.xlabel('Time (s)')
        plt.ylabel('Mean of CPU Cache Idle : Live')
        plt.title(self.pic_name)
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.savefig(os.path.join(self.PIC_DIR, self.pic_name  + '-Mean CPU Cache Idle.png'), dpi = 100)
        plt.show()
    
    def graph_mean_median_timeseries(self):
        x = list(range(len(self.dict_list)))
        means = list()
        medians = list()
        for hp_dict in self.dict_list:
            uselessness_list = [(cpu+1)/(live+1) for (live, cpu) in hp_dict.values()]
            means.append(np.mean(uselessness_list))
            medians.append(np.median(uselessness_list))
        # if self.deallocate_log:
        #     for coor in self.deallocate_log:
        #         plt.axvline(x=coor, c="skyblue", ls='--')
        #         pass
        plt.plot(x, means,label="mean")
        plt.plot(x, medians,label="median")
        plt.xlabel('Time (s)')
        plt.ylabel('Mean and Median of CPU Cache Idle : Live')
        plt.title(self.pic_name)
        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.savefig(os.path.join(self.PIC_DIR, self.pic_name  + '-Mean Median CPU Cache Idle.png'), dpi = 100)
        plt.show()

    def plot_stranded_percentage(self):
        textstr = '\n'.join((f"Max Requested HP {max(self.totalHugePageRequested)}",
                            f"Min Requested HP {min(self.totalHugePageRequested)}",
                            f"Max Stranded {max(self.totalStrandedPage)}",
                            f"Min Stranded {min(self.totalStrandedPage)}"))
        plt.plot(self.percentage, label="Percentage")
        plt.xlabel("time (s)")
        plt.ylabel("%")
        plt.title("% Huge Page Stranded in " + self.test_name)

        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.text(0.8, 0.9, textstr, fontsize = 12, bbox = dict(facecolor = 'white', alpha = 0.5))
        fig.savefig(os.path.join(self.PIC_DIR, self.test_name + '-HP-Stranded.png'), dpi = 100)
        plt.show()

    def plot_hugepage_usage(self):
        plt.plot(self.totalStrandedPage, label="Stranded Page")
        plt.plot(self.totalHugePageRequested, label="Requested")
        plt.plot(self.totalHugePageInUse, label="In Use")
        plt.plot(self.totalHugePageFree, label="Free")
        plt.legend()
        plt.xlabel("time (s)")
        plt.ylabel("num(s) page")
        plt.title("Huge Page Usage in " + self.test_name)
        
        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.savefig(os.path.join(self.PIC_DIR, self.test_name + '-HP-Usage.png'), dpi = 100)
        plt.show()


class HugeCache:
    def __init__(self, DIR, PIC_DIR, profile, test_name):
        self.DIR = DIR
        self.PIC_DIR = PIC_DIR
        self.pic_name = profile
        self.test_name = test_name

        self.pattern = "HugeCache: \d+ \/\ \d+ hugepages cached \/\ cache limit \(\d+.\d+ hit rate, \d+.\d+ overflow rate\)"

        self.current_hp_in_cache = []
        self.current_capacity = []

        self.get_data()

    def get_data(self):
        for filename in natsorted(os.listdir(self.DIR)):
            if filename.endswith(".txt"): 
                with open(self.DIR + '/' + filename) as file:
                    for info in file:
                        HugeCache = re.search(self.pattern, info)
                        if HugeCache:
                            count = 0
                            for word in HugeCache.group().split(' '):
                                if word.isdigit():
                                    if not count:
                                        self.current_hp_in_cache.append(word)
                                    elif count == 1:
                                        self.current_capacity.append(word)
                                        break
                                    count += 1
                            break
        self.plot()

    def plot(self):
        plt.plot(self.current_hp_in_cache, label="Available")
        plt.plot(self.current_capacity, label="Capacity")
        plt.legend()
        plt.xlabel("time (s)")
        plt.ylabel("num(s) page")
        plt.title("Huge Page Usage in " + self.test_name)
        plt.legend()
        fig = plt.gcf()
        fig.set_size_inches(18.5, 10.5)
        fig.savefig(os.path.join(self.PIC_DIR, self.test_name + '-HP-Usage.png'), dpi = 100)
        plt.show()


class Driver:
    def __init__(self, test_suite, tests, release_rates, dir, profile_name, drain_check_cycle):
        self.smem_dir = dir + "smem/" + profile_name + "/"
        self.stat_dir = dir + "stats_storage/" + profile_name + "/"
        self.log_dir = dir + "log/"
        self.pic_dir = dir + "pic/" + profile_name + "/"
        self.test_suite = test_suite

        try:
            os.mkdir(dir + "pic/")
        except FileExistsError:
            pass

        try:
            os.mkdir(self.pic_dir)
        except FileExistsError:
            pass

        self.profile_name = profile_name
        self.drain_check_cycle = drain_check_cycle
        self.tests = tests
        self.release_rates = release_rates
        self.deallocate_log = None
        
        self.run()
    
    def run(self):
        for test_name in self.tests:
            for rate in self.release_rates:
                current_stat_dir = self.stat_dir + test_name + "/" + rate + "/" + self.drain_check_cycle
                current_smem_dir = self.smem_dir + test_name + "/" + rate + "/" + self.drain_check_cycle
                if self.test_suite == "redis":
                    self.deallocate_log = Log(self.log_dir, self.profile_name + "-" + test_name + "-" + rate + "-" + self.drain_check_cycle).get_log()
                pic_name = self.profile_name + "-" + test_name + "-" + rate + "-" + self.drain_check_cycle
                HugeCache(current_stat_dir, self.pic_dir, pic_name, self.profile_name)

                # Benchmark_Stat(current_stat_dir, self.redis_pic_dir, pic_name, self.deallocate_log)
                # Memory_Stat(current_smem_dir, self.redis_pic_dir, pic_name, self.deallocate_log)
                # g = GraphHugePageStats(current_stat_dir, self.redis_pic_dir, pic_name, self.deallocate_log)
                # g.graph_mean_median_timeseries()
                # g.graph_mean_std_timeseries()

Driver(test_suite="mybench",
       tests=["Producer-Consumer"],
       release_rates=["0MB"],
       dir="/home/grads/t/tiendat.ng.cs/Documents/github_repos/tcmalloc/benchmarks/minh-custom-bench/",
       profile_name="Bravo",
       drain_check_cycle="1s")
    