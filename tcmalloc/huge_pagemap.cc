// Dat mod
# include "tcmalloc/huge_pagemap.h"

#include "tcmalloc/static_vars.h"


GOOGLE_MALLOC_SECTION_BEGIN
namespace tcmalloc {
namespace tcmalloc_internal {

void* MetaDataAlloc(size_t bytes) ABSL_EXCLUSIVE_LOCKS_REQUIRED(pageheap_lock) {
  return Static::arena().Alloc(bytes);
}

}  // namespace tcmalloc_internal
}  // namespace tcmalloc
GOOGLE_MALLOC_SECTION_END