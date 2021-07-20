// Dat mod


#ifndef TCMALLOC_HUGE_PAGEMAP_H_
#define TCMALLOC_HUGE_PAGEMAP_H_


#include "tcmalloc/common.h"
#include "tcmalloc/huge_pages.h"
#include "tcmalloc/static_vars.h"

#include <atomic>
#include <string>
// Dat mod ends

GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {

// typedef void* (*allocator_type)(size_t size);
void* MetaDataAlloc(size_t bytes);

// Storing 2^(8*sizeof(uintptr_t)) entries. Keep track of per-hugepage allocation stats Mapping Hugepage structs to corresponding stats.

// template <void* (Allocator)(size_t size)>
class HugePageMap {
    private:
        static constexpr uint32_t rootBits = (sizeof(uint32_t) / 2) * 8;
        static constexpr uint32_t rootLength = (uint32_t)1 << rootBits;
        static constexpr uint32_t leafBits = (sizeof(uint32_t) / 2) * 8;
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
            void AddIdleSize(int increment){
                idle_size_.fetch_add(increment, std::memory_order_relaxed);
            }

            int GetLiveSize() const{
                return live_size_.load(std::memory_order_relaxed);
            }
            int GetIdleSize() const{
                return idle_size_.load(std::memory_order_relaxed);
            }
            size_t GetFreeSize() const{
                return kHugePageSize - GetLiveSize() - GetIdleSize();
            }

            std::atomic<int> live_size_ = 0; // Amount being used by application
            std::atomic<int> idle_size_ = 0; // Amount sitting idly in CPU cache
        };

        struct Leaf {
            HugePageStats* huge_page_stats[leafLength];
        };

        Leaf* root_[rootLength];
    
    public:
        HugePageMap(): root_{} {}
        uint32_t get_i1(uintptr_t idx) const {
            return idx >> leafBits;
        }
        uint32_t get_i2(uintptr_t idx) const {
            return idx & (leafLength - 1);
        }

        void add_live_size(HugePage hp, int increment){
            uint32_t i1 = get_i1(hp.index());
            uint32_t i2 = get_i2(hp.index());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddLiveSize(increment);
        }

        void add_idle_size(HugePage hp, int increment){
            uint32_t i1 = get_i1(hp.index());
            uint32_t i2 = get_i2(hp.index());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            root_[i1]->huge_page_stats[i2]->AddIdleSize(increment);
        }

        int get_live_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.index());
            uint32_t i2 = get_i2(hp.index());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            return root_[i1]->huge_page_stats[i2]->GetLiveSize();
        }

        int get_idle_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.index());
            uint32_t i2 = get_i2(hp.index());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            return root_[i1]->huge_page_stats[i2]->GetIdleSize();
        }

        size_t get_free_size(HugePage hp)  {
            uint32_t i1 = get_i1(hp.index());
            uint32_t i2 = get_i2(hp.index());
            init_stats_if_necessary(i1, i2);
            ASSERT(root_[i1] != nullptr);
            return root_[i1]->huge_page_stats[i2]->GetFreeSize();
        }

        std::string get_huge_page_stats(HugePage hp){
            std::string ret = "";
            return ret;
        }
};



}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END

#endif
// Dat mod ends