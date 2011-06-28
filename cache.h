
#ifndef _CACHE_H_
#define _CACHE_H_

namespace policy {
  typedef enum {
    lru
  } type_t;
}

namespace access {
  typedef enum {
    read,
    write
  } type_t;
}

typedef struct {
  ulong         tag;
  char          valid, dirty;  
  cache_block_t *next, *previous;
} cache_block_t;

typedef struct {
  cache_blk_t *head, *tail;
} cache_set_t;

typedef struct {
  int            sets, block_size, associativity;
  policy::type_t policy;  
  ulong          accesses, hits, misses, replacements, writebacks;
  cache_set_t    *sets;
} cache_t;

void cache_create(int A, int B, int C, policy::type_t policy = policy::lru);
int  cache_access(cache_t *cache, ulong address, access::type_t access);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
