// Dat mod


#ifndef TCMALLOC_HUGE_PAGEMAP_H_
#define TCMALLOC_HUGE_PAGEMAP_H_


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
void* MetaDataAlloc(size_t bytes);

// Storing 2^(8*sizeof(uintptr_t)) entries. Keep track of per-hugepage allocation stats Mapping Hugepage structs to corresponding stats.

// template <void* (Allocator)(size_t size)>
class HugePageMap {
    private:
        static constexpr uint32_t totalBits = kAddressBits - kHugePageShift;
        static constexpr uint32_t rootBits = totalBits / 2;
        static constexpr uint32_t rootLength = (uint32_t)1 << rootBits;
        static constexpr uint32_t leafBits = totalBits - rootBits;
        static constexpr uint32_t leafLength = (uint32_t)1 << leafBits;
        // static void* Alloc(size_t size) ABSL_EXCLUSIVE_LOCKS_REQUIRED(pageheap_lock){
        //     return Static::arena().Alloc(size);
        // }

        void init_stats_if_necessary(uint32_t i1, uint32_t i2){
            if (root_[i1] == nullptr){
                Leaf* leaf = reinterpret_cast<Leaf*>(MetaDataAlloc(sizeof(Leaf)));
                root_[i1] = leaf;
            }
            ASSERT(root_[i1] != nullptr);
            if (root_[i1]->huge_page_stats[i2] == nullptr){
                HugePageStats* hp_stats = reinterpret_cast<HugePageStats*>(MetaDataAlloc(sizeof(HugePageStats)));
                root_[i1]->huge_page_stats[i2] = hp_stats;
            }
        }

        struct HugePageStats {
            void AddLiveSize(int increment){
                live_size_.fetch_add(increment, std::memory_order_relaxed);
            }
            void AddCPUCacheIdleSize(int increment){
                cpu_cache_idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }
            void AddCentralCacheIdleSize(int increment){
                central_cache_idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }

            int GetLiveSize() const{
                return live_size_.load(std::memory_order_relaxed);
            }
            int GetCPUCacheIdleSize() const{
                return cpu_cache_idle_size_.load(std::memory_order_relaxed);
            }
            int GetCentralCacheIdleSize() const{
                return central_cache_idle_size_.load(std::memory_order_relaxed);
            }
            size_t GetFreeSize() const{
                return kHugePageSize - GetLiveSize() - GetCPUCacheIdleSize() - GetCentralCacheIdleSize();
            }

            std::atomic<int64_t> live_size_ = 0; // Amount being used by application
            std::atomic<int64_t> cpu_cache_idle_size_ = 0; // Amount sitting idly in CPU cache
            std::atomic<int64_t> central_cache_idle_size_ = 0; // Amount sitting idly in CPU cache
        };

        struct Leaf {
            HugePageStats* huge_page_stats[leafLength];
        };

        Leaf* root_[rootLength];
    
    public:
        HugePageMap(): root_{} {}
        uint32_t get_i1(void*  addr) const {
            return reinterpret_cast<uintptr_t>(addr) >> (kHugePageShift + leafBits);
        }
        uint32_t get_i2(void*  addr) const {
            return (reinterpret_cast<uintptr_t>(addr) >> kHugePageShift) & (leafLength - 1);
        }

        void add_live_size(HugePage hp, int increment){
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddLiveSize(increment);
        }

        void add_cpu_cache_idle_size(HugePage hp, int increment){
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddCPUCacheIdleSize(increment);
        }

        void add_central_cache_idle_size(HugePage hp, int increment){
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddCentralCacheIdleSize(increment);
        }

        int get_live_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            return root_[i1]->huge_page_stats[i2]->GetLiveSize();
        }

        int get_cpu_cache_idle_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            return root_[i1]->huge_page_stats[i2]->GetCPUCacheIdleSize();
        }
        
        int get_central_cache_idle_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            return root_[i1]->huge_page_stats[i2]->GetCentralCacheIdleSize();
        }

        size_t get_free_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.start_addr());
            uint32_t i2 = get_i2(hp.start_addr());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
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
                    hp_addr = reinterpret_cast<void*>((((uint64_t)i1 << leafBits) | (uint64_t)i2) << kHugePageShift);  
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