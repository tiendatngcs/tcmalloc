// Dat mod


#ifndef TCMALLOC_HUGE_PAGEMAP_H_
#define TCMALLOC_HUGE_PAGEMAP_H_

#include "tcmalloc/huge_pages.h"

#include <atomic>
#include <string>
// Dat mod ends

GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {
// Storing 2^(8*sizeof(uintptr_t)) entries. Keep track of per-hugepage allocation stats Mapping Hugepage structs to corresponding stats.
class HugePageMap {
    static constexpr uint32_t rootBits = (sizeof(uint32_t) / 2) * 8;
    static constexpr uint32_t rootLength = (uint32_t)1 << rootBits;
    static constexpr uint32_t leafBits = (sizeof(uint32_t) / 2) * 8;
    static constexpr uint32_t leafLength = (uint32_t)1 << leafBits;
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
        uint32_t get_root_idx(uintptr_t idx) const {
            return idx >> leafBits;
        }
        uint32_t get_leaf_idx(uintptr_t idx) const {
            return idx & (leafLength - 1);
        }

        void add_live_size(HugePage hp, int increment){
            uint32_t root_idx = get_root_idx(hp.index());
            uint32_t leaf_idx = get_leaf_idx(hp.index());
            root_[root_idx]->huge_page_stats[leaf_idx]->AddLiveSize(increment);
        }

        void add_idle_size(HugePage hp, int increment){
            uint32_t root_idx = get_root_idx(hp.index());
            uint32_t leaf_idx = get_leaf_idx(hp.index());
            root_[root_idx]->huge_page_stats[leaf_idx]->AddIdleSize(increment);
        }

        int get_live_size(HugePage hp)  {
            uint32_t root_idx = get_root_idx(hp.index());
            uint32_t leaf_idx = get_leaf_idx(hp.index());
            return root_[root_idx]->huge_page_stats[leaf_idx]->GetLiveSize();
        }

        int get_idle_size(HugePage hp)  {
            uint32_t root_idx = get_root_idx(hp.index());
            uint32_t leaf_idx = get_leaf_idx(hp.index());
            return root_[root_idx]->huge_page_stats[leaf_idx]->GetIdleSize();
        }

        size_t get_free_size(HugePage hp)  {
            uint32_t root_idx = get_root_idx(hp.index());
            uint32_t leaf_idx = get_leaf_idx(hp.index());
            return root_[root_idx]->huge_page_stats[leaf_idx]->GetFreeSize();
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