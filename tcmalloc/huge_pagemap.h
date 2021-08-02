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

            void AddLiveSize(int64_t increment){
                live_size_.fetch_add(increment);
                // live_size_.fetch_add(increment, std::memory_order_relaxed);
            }
            void AddCPUCacheIdleSize(int64_t increment){
                cpu_cache_idle_size_.fetch_add(increment);
                // cpu_cache_idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }
            void AddCentralCacheIdleSize(int64_t increment){
                central_cache_idle_size_.fetch_add(increment);
                // central_cache_idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }

            int64_t GetLiveSize() const{
                return live_size_.load();
                // return live_size_.load(std::memory_order_relaxed);
            }
            int64_t GetCPUCacheIdleSize() const{
                return cpu_cache_idle_size_.load();
                // return cpu_cache_idle_size_.load(std::memory_order_relaxed);
            }
            int64_t GetCentralCacheIdleSize() const{
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

            std::atomic<int64_t> live_size_; // Amount being used by application
            std::atomic<int64_t> cpu_cache_idle_size_; // Amount sitting idly in CPU cache
            std::atomic<int64_t> central_cache_idle_size_; // Amount sitting idly in CPU cache
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
    
        uint32_t get_i1(void*  addr) const {
            return reinterpret_cast<uintptr_t>(addr) >> (kHugePageShift + leafBits);
        }
        uint32_t get_i2(void*  addr) const {
            return (reinterpret_cast<uintptr_t>(addr) >> kHugePageShift) & (leafLength - 1);
        }
        uint32_t get_i1(HugePage hp) const {
            return get_i1(hp.start_addr());
        }
        uint32_t get_i2(HugePage hp) const {
            return get_i2(hp.start_addr());
        }
        HugePage get_hp(uint32_t i1, uint32_t  i2) const {
            return HugePageContaining(reinterpret_cast<void*>((((uint64_t)i1 << leafBits) | (uint64_t)i2) << kHugePageShift));
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
            uint32_t i1 = get_i1(hp);
            uint32_t i2 = get_i2(hp);
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

        void add_live_size(HugePage hp, int64_t increment){
            absl::base_internal::SpinLockHolder h(&lock_);
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddLiveSize(increment);
            // Log(kLog, __FILE__, __LINE__, "Changing live size of hp_addr", hp.start_addr(), increment, get_live_size(hp));
            // ASSERT(get_live_size(hp) >= 0);
            if (get_free_size(hp) < 0){
                // Log(kLog, __FILE__, __LINE__, "Changing live size of hp_addr", hp.start_addr(), increment, get_live_size(hp));
                Log(kLog, __FILE__, __LINE__, "Total free of hp", hp.start_addr(), get_free_size(hp));
                // Log(kLog, __FILE__, __LINE__, "Negative free size detected____________________");
                // Log(kLog, __FILE__, __LINE__, "Operation: add live");
                // Log(kLog, __FILE__, __LINE__, "live:", get_live_size(hp));
                // Log(kLog, __FILE__, __LINE__, "cpu_cache_idle:", get_cpu_cache_idle_size(hp));
                // Log(kLog, __FILE__, __LINE__, "central_cache_idle:", get_central_cache_idle_size(hp));
                // Log(kLog, __FILE__, __LINE__, "free:", get_free_size(hp));
                // Log(kLog, __FILE__, __LINE__, "--------------------------------------------------------------");
            }
            // ASSERT(get_free_size(hp) >= 0);
        }

        void add_cpu_cache_idle_size(HugePage hp, int64_t increment){
            absl::base_internal::SpinLockHolder h(&lock_);
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddCPUCacheIdleSize(increment);
            // Log(kLog, __FILE__, __LINE__, "Changing cpu cache idle size of hp_addr", hp.start_addr(), increment, get_cpu_cache_idle_size(hp));
            // ASSERT(get_cpu_cache_idle_size(hp) >= 0);
            if (get_free_size(hp) < 0){
                // Log(kLog, __FILE__, __LINE__, "Changing cpu cache idle size of hp_addr", hp.start_addr(), increment, get_cpu_cache_idle_size(hp));
                Log(kLog, __FILE__, __LINE__, "Total free of hp", hp.start_addr(), get_free_size(hp));
                // Log(kLog, __FILE__, __LINE__, "Negative free size detected____________________");
                // Log(kLog, __FILE__, __LINE__, "Operation: add cpu cache idle");
                // Log(kLog, __FILE__, __LINE__, "live:", get_live_size(hp));
                // Log(kLog, __FILE__, __LINE__, "cpu_cache_idle:", get_cpu_cache_idle_size(hp));
                // Log(kLog, __FILE__, __LINE__, "central_cache_idle:", get_central_cache_idle_size(hp));
                // Log(kLog, __FILE__, __LINE__, "free:", get_free_size(hp));
                // Log(kLog, __FILE__, __LINE__, "--------------------------------------------------------------");
            }
            // ASSERT(get_free_size(hp) >= 0);
        }

        void add_central_cache_idle_size(HugePage hp, int64_t increment){
            absl::base_internal::SpinLockHolder h(&lock_);
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddCentralCacheIdleSize(increment);
            // Log(kLog, __FILE__, __LINE__, "Changing central cache idle size of hp_addr", hp.start_addr(), increment, get_central_cache_idle_size(hp));
            // ASSERT(get_central_cache_idle_size(hp) >= 0);
            if (get_free_size(hp) < 0){
                Log(kLog, __FILE__, __LINE__, "Total free of hp", hp.start_addr(), get_free_size(hp));
                // Log(kLog, __FILE__, __LINE__, "Negative free size detected____________________");
                // Log(kLog, __FILE__, __LINE__, "Operation: add central cache idle");
                // Log(kLog, __FILE__, __LINE__, "live:", get_live_size(hp));
                // Log(kLog, __FILE__, __LINE__, "cpu_cache_idle:", get_cpu_cache_idle_size(hp));
                // Log(kLog, __FILE__, __LINE__, "central_cache_idle:", get_central_cache_idle_size(hp));
                // Log(kLog, __FILE__, __LINE__, "free:", get_free_size(hp));
                // Log(kLog, __FILE__, __LINE__, "--------------------------------------------------------------");
            }
            // ASSERT(get_free_size(hp) >= 0);
        }

        int32_t get_live_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            if (root_[i1] == nullptr) return 0;
            if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetLiveSize();
        }

        int32_t get_cpu_cache_idle_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            if (root_[i1] == nullptr) return 0;
            if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetCPUCacheIdleSize();
        }
        
        int32_t get_central_cache_idle_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            if (root_[i1] == nullptr) return 0;
            if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetCentralCacheIdleSize();
        }

        int32_t get_free_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            if (root_[i1] == nullptr) return 0;
            if (root_[i1]->huge_page_stats[i2] == nullptr) return 0;
            return root_[i1]->huge_page_stats[i2]->GetFreeSize();
        }

        void PrintStats(Printer *out){
            // uintptr_t hp_addr;
            void* hp_addr;
            out->printf("------------------------------------------------\n");
            out->printf("HugePageMap Stats. Per-HugePage memory usage:\n");
            out->printf("------------------------------------------------\n");
            for (uint32_t i1 = 0; i1 < rootLength; i1++){
                if (root_[i1] != nullptr){
                for (uint32_t i2 = 0; i2 < leafLength; i2++){
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
};



}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END

#endif
// Dat mod ends