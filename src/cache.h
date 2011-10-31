
#ifndef _CACHE_H_
#define _CACHE_H_

#include "hash.h"
#include "types.h"

#define MINIMUM_CACHE_LINE_SIZE 2

namespace cache_operation {
  typedef enum {
    read,
    write
  } type_t;
}

namespace miss {
  typedef enum {
    pure,
    compulsory,
    conflict
  } type_t;
}

typedef struct cache_node_tag {
  size_t         tag;
  void           *key;
  char           *name;
  cache_node_tag *next, *older, *newer;
} cache_node_t;

typedef struct {
  ulong read_hits;
  ulong read_misses;
  ulong compulsory_read_misses;
  ulong conflict_read_misses;
  ulong write_hits;
  ulong write_misses;
  ulong compulsory_write_misses;
  ulong conflict_write_misses;
} cache_statistics_t;

typedef struct cache_line_lifetime_tag {
  size_t                  start, end;
  char                    *name;
  cache_line_lifetime_tag *next;
} cache_line_lifetime_t;

typedef struct {
  size_t             entries, lines;
  size_t             cache_size, cache_line_size;
  cache_node_t       **nodes, *mru, *lru;
  uint               ticks;
  hash_table_t       *addresses;
  cache_statistics_t statistics[26];
  bool               has_statistics[26];
} cache_t;

cache_t* cache_malloc(size_t max_size, size_t line_size);
void cache_free(cache_t *table);
void cache_supported(cache_t *cache);
void cache_access(cache_t *cache, void const *key, cache_operation::type_t access, char const *format, ...);
char const* cache_operation_to_string(cache_operation::type_t access);
void cache_print_profile(cache_t *table);
void cache_debug(cache_t *cache);

#endif /* _CACHE_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
