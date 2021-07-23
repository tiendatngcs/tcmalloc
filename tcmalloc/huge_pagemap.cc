// Dat mod
#include "tcmalloc/huge_pagemap.h"
#include "tcmalloc/internal/logging.h"
#include "tcmalloc/static_vars.h"


GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {

void* MetaDataAlloc(size_t bytes) ABSL_EXCLUSIVE_LOCKS_REQUIRED(pageheap_lock) {
  return Static::arena().Alloc(bytes);
}

void HugePageMap::PrintStats(Printer *out){
  uintptr_t hp_addr;
  out->printf("------------------------------------------------\n");
  out->printf("HugePageMap Stats. Per-HugePage memory usage:\n");
  out->printf("------------------------------------------------\n");
  for (uint32_t i1 = 0; i1 < rootLength; i1++){
    if (root_[i1] != nullptr){
      for (uint32_t i2 = 0; i2 < leafLength; i2++){
        if (root_[i1]->huge_page_stats[i2] != nullptr){
          hp_addr = ((i1 << leafBits) | i2) << kHugePageShift;  
          out->printf("HugePage at addr %d\n", hp_addr);
          out->printf("\tLive size: %12" PRIu64 " bytes\n", root_[i1]->huge_page_stats[i2]->GetLiveSize());
          out->printf("\tIdle size: %12" PRIu64 " bytes\n", root_[i1]->huge_page_stats[i2]->GetIdleSize());
          out->printf("\tFree size: %12" PRIu64 " bytes\n", root_[i1]->huge_page_stats[i2]->GetFreeSize());
          out->printf("------------------\n");
        }
      }
    }
  }
}

}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END