// Dat mod


#ifndef TCMALLOC_HUGE_PAGEMAP_H_
#define TCMALLOC_HUGE_PAGEMAP_H_


#include "absl/base/internal/spinlock.h"
#include "tcmalloc/common.h"
#include "tcmalloc/huge_pages.h"
#include "tcmalloc/internal/logging.h"

#include <atomic>
#include <inttypes.h>
#include <iostream>
#include <string>

GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {

// typedef void* (*allocator_type)(size_t size);

// Storing 2^(8*sizeof(uintptr_t)) entries. Keep track of per-hugepage allocation stats Mapping Hugepage structs to corresponding stats.

// template <void* (Allocator)(size_t size)>
class HugePageMap {
    private:
        static constexpr uint32_t totalBits = kAddressBits - kHugePageShift;
        static constexpr uint32_t rootBits = totalBits / 2;
        static constexpr uint32_t rootLength = (uint32_t)1 << rootBits;
        static constexpr uint32_t leafBits = totalBits - rootBits;
        static constexpr uint32_t leafLength = (uint32_t)1 << leafBits;

        // void init_stats_if_necessary(uint32_t i1, uint32_t i2){
        //     if (root_[i1] == nullptr){
        //         Leaf* leaf = reinterpret_cast<Leaf*>(MetaDataAlloc(sizeof(Leaf)));
        //         memset(leaf, 0, sizeof(*leaf));
        //         root_[i1] = leaf;
        //     }
        //     ASSERT(root_[i1] != nullptr);
        //     if (root_[i1]->huge_page_stats[i2] == nullptr){
        //         HugePageStats* hp_stats = reinterpret_cast<HugePageStats*>(MetaDataAlloc(sizeof(HugePageStats)));
        //         hp_stats->Init();
        //         root_[i1]->huge_page_stats[i2] = hp_stats;
        //         //printf("Inint Hp stats, %ld, %ld, %ld, %lu", hp_stats->GetLiveSize(), hp_stats->GetCPUCacheIdleSize(), hp_stats->GetCentralCacheIdleSize(), hp_stats->GetFreeSize());
        //     }
        // }

        struct HugePageStats {
            HugePageStats() = default;

            void AddLiveSize(int32_t increment){
                live_size_.fetch_add(increment);
                // live_size_.fetch_add(increment, std::memory_order_relaxed);
            }
            void AddCPUCacheIdleSize(int32_t increment){
                cpu_cache_idle_size_.fetch_add(increment);
                // cpu_cache_idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }
            void AddCentralCacheIdleSize(int32_t increment){
                central_cache_idle_size_.fetch_add(increment);
                // central_cache_idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }

            int32_t GetLiveSize() const{
                return live_size_.load();
                // return live_size_.load(std::memory_order_relaxed);
            }
            int32_t GetCPUCacheIdleSize() const{
                return cpu_cache_idle_size_.load();
                // return cpu_cache_idle_size_.load(std::memory_order_relaxed);
            }
            int32_t GetCentralCacheIdleSize() const{
                return central_cache_idle_size_.load();
                // return central_cache_idle_size_.load(std::memory_order_relaxed);
            }
            size_t GetFreeSize() const{
                return kHugePageSize + /*(kPageSize * 32)*/ - GetLiveSize() - GetCPUCacheIdleSize() - GetCentralCacheIdleSize();
            }

            void Init() {
                live_size_ = 0;
                cpu_cache_idle_size_ = 0;
                central_cache_idle_size_ = 0;
            } 

            std::atomic<int32_t> live_size_; // Amount being used by application
            std::atomic<int32_t> cpu_cache_idle_size_; // Amount sitting idly in CPU cache
            std::atomic<int32_t> central_cache_idle_size_; // Amount sitting idly in CPU cache
        };

        struct Leaf {
            HugePageStats* huge_page_stats[leafLength];
            ~Leaf(){
                Log(kLog, __FILE__, __LINE__, "Deleting leaf node.......................");
            }
        };

        Leaf* root_[rootLength];

        absl::base_internal::SpinLock lock_;

        void* MetaDataAlloc(size_t bytes);
    
        uintptr_t get_i1(void*  addr) const {
            return reinterpret_cast<uintptr_t>(addr) >> (kHugePageShift + leafBits);
        }
        uintptr_t get_i2(void*  addr) const {
            return (reinterpret_cast<uintptr_t>(addr) >> kHugePageShift) & (leafLength - 1);
        }
        uintptr_t get_i1(HugePage hp) const {
            return get_i1(hp.start_addr());
        }
        uintptr_t get_i2(HugePage hp) const {
            return get_i2(hp.start_addr());
        }
        HugePage get_hp(uintptr_t i1, uintptr_t  i2) const {
            return HugePageContaining(reinterpret_cast<void*>(((i1 << leafBits) | i2) << kHugePageShift));
        }

        static SizeMap& sizemap();

    public:
        HugePageMap() = default;

        void Init(){
            // Log(kLog, __FILE__, __LINE__, "New HugePageMap instance created ............");
        }

        ~HugePageMap(){
            // Log(kLog, __FILE__, __LINE__, "HugePageMap instance destroyed .............");
        }

        void init_huge_page_stats(HugePage hp){
            absl::base_internal::SpinLockHolder h(&lock_);
            uintptr_t i1 = get_i1(hp);
            uintptr_t i2 = get_i2(hp);
            if (root_[i1] == nullptr){
                Leaf* leaf = reinterpret_cast<Leaf*>(MetaDataAlloc(sizeof(Leaf)));
                *leaf = {}; // Init leaf
                root_[i1] = leaf;
            }
            ASSERT(root_[i1]->huge_page_stats[i2] == nullptr);
            HugePageStats* hp_stats = reinterpret_cast<HugePageStats*>(MetaDataAlloc(sizeof(HugePageStats)));
            hp_stats->Init(); // Init leaf
            root_[i1]->huge_page_stats[i2] = hp_stats;
        }

        void add_live_size(HugePage hp, int32_t increment){
            absl::base_internal::SpinLockHolder h(&lock_);
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddLiveSize(increment);
            // Log(kLog, __FILE__, __LINE__, "Changing live size of hp_addr", hp.start_addr(), increment, get_live_size(hp));
            // ASSERT(get_live_size(hp) >= 0);
            // if (get_free_size(hp) < 0){
            //     Log(kLog, __FILE__, __LINE__, "Changing live size of hp_addr", hp.start_addr(), increment, get_live_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "Total free of hp", hp.start_addr(), get_free_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "Negative free size detected____________________");
            //     Log(kLog, __FILE__, __LINE__, "live:", get_live_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "cpu_cache_idle:", get_cpu_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "central_cache_idle:", get_central_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "free:", get_free_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "--------------------------------------------------------------");
            // }
            ASSERT(get_free_size(hp) >= 0);
        }

        void add_cpu_cache_idle_size(HugePage hp, int32_t increment){
            absl::base_internal::SpinLockHolder h(&lock_);
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddCPUCacheIdleSize(increment);
            // Log(kLog, __FILE__, __LINE__, "Changing CPU cache idle size:", hp.start_addr(), increment, get_cpu_cache_idle_size(hp));
            // Log(kLog, __FILE__, __LINE__, "i1 i2:", i1, i2);
            // Log(kLog, __FILE__, __LINE__, "HP addr:", hp.start_addr(), get_hp(i1, i2).start_addr());
            // Log(kLog, __FILE__, __LINE__, "Changing cpu cache idle size of hp_addr", hp.start_addr(), increment, get_cpu_cache_idle_size(hp));
            // ASSERT(get_cpu_cache_idle_size(hp) >= 0);
            // if (get_free_size(hp) < 0){
            //     Log(kLog, __FILE__, __LINE__, "Changing cpu cache idle size of hp_addr", hp.start_addr(), increment, get_cpu_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "Total free of hp", hp.start_addr(), get_free_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "Negative free size detected____________________");
            //     Log(kLog, __FILE__, __LINE__, "live:", get_live_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "cpu_cache_idle:", get_cpu_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "central_cache_idle:", get_central_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "free:", get_free_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "--------------------------------------------------------------");
            // }
            ASSERT(get_free_size(hp) >= 0);
        }

        void add_central_cache_idle_size(HugePage hp, int32_t increment){
            absl::base_internal::SpinLockHolder h(&lock_);
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddCentralCacheIdleSize(increment);
            // ASSERT(get_central_cache_idle_size(hp) >= 0);
            // if (get_free_size(hp) < 0){
            //     Log(kLog, __FILE__, __LINE__, "Changing central cache idle size of hp_addr", hp.start_addr(), increment, get_central_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "Total free of hp", hp.start_addr(), get_free_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "Negative free size detected____________________");
            //     Log(kLog, __FILE__, __LINE__, "live:", get_live_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "cpu_cache_idle:", get_cpu_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "central_cache_idle:", get_central_cache_idle_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "free:", get_free_size(hp));
            //     Log(kLog, __FILE__, __LINE__, "--------------------------------------------------------------");
            // }
            ASSERT(get_free_size(hp) >= 0);
        }

        int32_t get_live_size(HugePage hp)  {
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            ASSERT(root_[i1]->huge_page_stats[i2] != nullptr);
            // if (root_[i1] == nullptr){
            //     Log(kLog, __FILE__, __LINE__, "Huge page does not exist at ", hp.start_addr());
            //     return 0;
            // }
            // if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetLiveSize();
        }

        int32_t get_cpu_cache_idle_size(HugePage hp)  {
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            ASSERT(root_[i1]->huge_page_stats[i2] != nullptr);
            // if (root_[i1] == nullptr){
            //     Log(kLog, __FILE__, __LINE__, "Huge page does not exist at ", hp.start_addr());
            //     return 0;
            // }
            // if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetCPUCacheIdleSize();
        }
        
        int32_t get_central_cache_idle_size(HugePage hp)  {
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            ASSERT(root_[i1]->huge_page_stats[i2] != nullptr);
            // if (root_[i1] == nullptr){
            //     Log(kLog, __FILE__, __LINE__, "Huge page does not exist at ", hp.start_addr());
            //     return 0;
            // }
            // if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetCentralCacheIdleSize();
        }

        int32_t get_free_size(HugePage hp)  {
            uintptr_t i1 = get_i1(hp.start_addr());
            uintptr_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            ASSERT(root_[i1]->huge_page_stats[i2] != nullptr);
            // if (root_[i1] == nullptr){
            //     Log(kLog, __FILE__, __LINE__, "Huge page does not exist at ", hp.start_addr());
            //     return 0;
            // }
            // if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetFreeSize();
        }

        double get_hp_idle_percentage(HugePage hp) {
            int32_t cpu_cache_idle_size = get_cpu_cache_idle_size(hp);
            int32_t live_size = get_live_size(hp);
            if (live_size + cpu_cache_idle_size == 0) return 0;
            return double (cpu_cache_idle_size) / (live_size + cpu_cache_idle_size);
        }

        void PrintStats(Printer *out){
            // uintptr_t hp_addr;
            absl::base_internal::SpinLockHolder h(&lock_);
            void* hp_addr;
            out->printf("------------------------------------------------\n");
            out->printf("HugePageMap Stats. Per-HugePage memory usage:\n");
            out->printf("------------------------------------------------\n");
            for (uintptr_t i1 = 0; i1 < rootLength; i1++){
                if (root_[i1] != nullptr){
                    for (uintptr_t i2 = 0; i2 < leafLength; i2++){
                        if (root_[i1]->huge_page_stats[i2] != nullptr){
                            hp_addr = get_hp(i1, i2).start_addr();
                            out->printf("HugePage at addr %p\n"
                            "\tLive size: %12d bytes\n"
                            "\tCPU Cache Idle size: %12d bytes\n"
                            "\tCentral Cache Idle size: %12d bytes\n"
                            "\tFree size: %12d bytes\n"
                            "------------------\n",
                            hp_addr,
                            get_live_size(HugePageContaining(hp_addr)),
                            get_cpu_cache_idle_size(HugePageContaining(hp_addr)),
                            get_central_cache_idle_size(HugePageContaining(hp_addr)),
                            get_free_size(HugePageContaining(hp_addr)));
                        }
                    }
                }
            }
        }

        double get_mean_degree_of_uselessness() {
            double total_dou = 0;
            int count = 0;
            for (uintptr_t i1 = 0; i1 < rootLength; i1++) {
                if (root_[i1] != nullptr) {
                    for (uintptr_t i2 = 0; i2 < leafLength; i2++) {
                        if (root_[i1]->huge_page_stats[i2] != nullptr) {
                            void* hp_addr = get_hp(i1, i2).start_addr();
                            int32_t cpu_cache_idle = get_cpu_cache_idle_size(HugePageContaining(hp_addr));
                            int32_t live_size = get_live_size(HugePageContaining(hp_addr));
                            total_dou += cpu_cache_idle / (live_size + 1);
                            count += 1;
                        }
                    }
                }
            }
            return total_dou / count;
        }

        bool existing_a_hp_breach_idle_percentage_threshold(double THRESHOLD) {
            HugePage hp;
            int count = 0;
            for (uintptr_t i1 = 0; i1 < rootLength; i1++) {
                if (root_[i1] != nullptr) {
                    for (uintptr_t i2 = 0; i2 < leafLength; i2++) {
                        if (root_[i1]->huge_page_stats[i2] != nullptr) {
                            hp = get_hp(i1, i2);
                            if (get_hp_idle_percentage(hp) >= THRESHOLD) return true;
                        }
                    }
                }
            }
            return false;
        }

        bool existing_a_hp_with_zero_live() {
            HugePage hp;
            int count = 0;
            for (uintptr_t i1 = 0; i1 < rootLength; i1++) {
                if (root_[i1] != nullptr) {
                    for (uintptr_t i2 = 0; i2 < leafLength; i2++) {
                        if (root_[i1]->huge_page_stats[i2] != nullptr) {
                            hp = get_hp(i1, i2);
                            if (get_live_size(hp) == 0 && get_cpu_cache_idle_size(hp) != 0) return true;
                        }
                    }
                }
            }
            return false;
        }
};
}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END

#endif
// Dat mod ends