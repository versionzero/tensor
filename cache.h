
#ifndef _CACHE_H_
#define _CACHE_H_

#include "hash.h"
#include "types.h"
#include <time.h>

#define MINIMUM_CACHE_LINE_SIZE 16

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
  ulong replacements;
} cache_statistics_t;

typedef size_t (*tag_function_t)(void const*);

typedef struct {
  size_t               size, max_size, line_size;
  cache_node_t         **nodes, *mru, *lru;
  cache_statistics_t   statistics;
  hash_table_t         *addresses;
  hash_function_t      hasher;
  tag_function_t       tagger;
  compare_function_t   comparator;
  duplicate_function_t duplicator;
  free_function_t      freer;
} cache_t;

cache_t* cache_malloc(size_t max_size, size_t line_size);
void cache_free(cache_t *table);
void cache_supported(cache_t *cache);
void cache_access(cache_t *cache, void const *key, cache_operation::type_t access);
char const* cache_operation_to_string(cache_operation::type_t access);
void cache_print_statistics(cache_t *table);
void cache_debug(cache_t *cache);

#endif /* _CACHE_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
