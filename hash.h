
#ifndef _HASH_H_
#define _HASH_H_

#include "types.h"
#include <time.h>

typedef struct hash_table_node_tag {
  void                *key, *data;
  hash_table_node_tag *next;
} hash_table_node_t;

typedef size_t (*hash_function_t)(void const*);
typedef size_t (*compare_function_t)(void const*, void const*);
typedef void*  (*duplicate_function_t)(void const*);
typedef void   (*free_function_t)(void*);

typedef struct {
  size_t               size, max_size;
  hash_table_node_t    **nodes;
  hash_function_t      hasher;
  compare_function_t   comparator;
  duplicate_function_t duplicator;
  free_function_t      freer;
} hash_table_t;

hash_table_t* hash_table_malloc(size_t max_size);
void hash_table_free(hash_table_t *table);
void hash_table_remove(hash_table_t *table, void const *key);
bool hash_table_insert(hash_table_t *table, void const *key, void *data);
hash_table_node_t* hash_table_find(hash_table_t *table, void const *key);
void hash_table_debug(hash_table_t *table);

#endif /* _HASH_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
