// Dat mod
#include "tcmalloc/common.h"
#include "tcmalloc/huge_pagemap.h"
#include "tcmalloc/static_vars.h"


GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {

void* HugePageMap::MetaDataAlloc(size_t bytes) ABSL_EXCLUSIVE_LOCKS_REQUIRED(pageheap_lock) {
  return Static::arena().Alloc(bytes);
}

SizeMap& HugePageMap::sizemap() {
  return Static::sizemap();
}



}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END