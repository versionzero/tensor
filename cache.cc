
#include "cache.h"

void
cache_create(int A, int B, int C, policy::type_t policy)
{
  /*
    create cache based on A, B, and C
    allocate sets
    allocate blocks in each set
    remember the replacement policy setting
    initialize statistics data
  */
}

int
cache_access(cache_t *cache, ulong address, access::type_t access)
{
  /*
    based on address determine the set to access in cp
    examine blocks in the set to check hit/miss
    if miss, determine the victim in the set to replace
    if LRU is used, update the block list
  */
}
