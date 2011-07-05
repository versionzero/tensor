/*
  Modified on Jun 29th, 2011.  Hash bashed LRU cache simulator.
  
  Original:
  
  Copyright (c) 2011 the authors listed at the following URL, and/or
  the authors of referenced articles or incorporated external code:
  http://en.literateprograms.org/Hash_table_(C)?action=history&offset=20100620072342
  
  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation files
  (the "Software"), to deal in the Software without restriction,
  including without limitation the rights to use, copy, modify, merge,
  publish, distribute, sublicense, and/or sell copies of the Software,
  and to permit persons to whom the Software is furnished to do so,
  subject to the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
  
  Retrieved from: http://en.literateprograms.org/Hash_table_(C)?oldid=16749
*/

#include "address.h"
#include "cache.h"
#include "error.h"
#include "memory.h"
#include "utility.h"
#include <stdio.h>
#include <string.h>

extern bool verbose;

size_t cache_memory_address_hash(void const *address);
size_t cache_memory_address_tag(void const *address);
size_t cache_memory_address_compare(void const *a, void const *b);
void*  cache_memory_address_duplicate(void const *address);
void   cache_memory_address_free(void *address);

/* Ok, so the whole idea is to try and simulate an "ideal" cache here.
   We use a least-recently-used (LRU) eviction policy, as it has been
   shown to perform closest to the clairvoyant replacement policy
   (OPT).
   
   We use a fusion of a hash table and a linked list.  The hash table
   allows for fast lookups.  The linked list maintains the access
   order. This was probably overkill for what we are doing, but
   because we will be running 100s, even 1000s, of iterations, it has
   the nice effect of not destroying my machine and exhausting
   patience. */


cache_t*
cache_malloc(size_t max_size, size_t line_size)
{
  cache_t *cache;
  
  debug("cache_malloc(max_size=%d, line_size=%d)\n", max_size, line_size);
  
  cache             = MALLOC(cache_t);
  cache->nodes      = MALLOC_N(cache_node_t*, max_size);
  cache->addresses  = hash_table_malloc(max_size);
  cache->max_size   = max_size;
  cache->line_size  = line_size;
  cache->size       = 0;
  cache->mru        = NULL;
  cache->lru        = NULL;
  cache->hasher     = &cache_memory_address_hash;
  cache->tagger     = &cache_memory_address_tag;
  cache->comparator = &cache_memory_address_compare;
  cache->duplicator = &cache_memory_address_duplicate;
  cache->freer      = &cache_memory_address_free;
  
  memset(cache->nodes, 0, max_size*sizeof(cache_node_t*));
  
  cache->statistics.read_hits               = 0;
  cache->statistics.read_misses             = 0;
  cache->statistics.compulsory_read_misses  = 0;
  cache->statistics.write_hits              = 0;
  cache->statistics.write_misses            = 0;
  cache->statistics.compulsory_write_misses = 0;
  cache->statistics.replacements            = 0;
  
  debug("cache_malloc: cache=0x%x\n", cache);
  
  return cache;
}

void
cache_free(cache_t *cache)
{
  size_t       i;
  cache_node_t *node, *next;
  
  debug("cache_free(cache=0x%x)\n", cache);
  
  for (i = 0; i < cache->max_size; ++i) {
    for (node = cache->nodes[i]; node; node = next) {
	next = node->next;
	cache->freer(node->key);
	safe_free(node);
    }
  }
  
  hash_table_free(cache->addresses);
  safe_free(cache->nodes);
  safe_free(cache);
}

size_t
cache_bucket(cache_t *cache, void const *key)
{
  return cache->hasher(key) % cache->max_size;
}

size_t
cache_bucket_size(cache_t *cache, size_t hash)
{
  size_t       size;
  cache_node_t *node;
  
  size = 0;
  
  for (node = cache->nodes[hash]; node; node = node->next) {
    size++;
  }
  
  return size;
}

void
cache_remove(cache_t *cache, void const *key)
{
  cache_node_t *node, *previous;
  size_t       hash;
  
  debug("cache_remove(cache=0x%x, key=0x%x)\n", cache, key);
  
  hash     = cache_bucket(cache, key);
  node     = cache->nodes[hash];
  previous = NULL;
  
  debug("cache_remove: hash=%d ~> root-node=0x%x\n", hash, node);
  
  while (node) {
    if (0 == cache->comparator(node->key, key)) {
      /* found */
      debug("cache_remove: hash=%d => node=0x%x\n", hash, node);
      if (previous) {
	if (previous != node->next) {
	  previous->next = node->next;
	} else {
	  previous->next = NULL;
	}
      } else {
	cache->nodes[hash] = node->next;
      }
      cache->freer(node->key);
      safe_free(node);
      cache->size--;
      return;
    }
    previous = node;
    node     = node->next;
  }
  
  die("Failed to remove key 0x%x; it was not found.\n", key);
}

void
cache_update_impl(cache_t *cache, cache_node_t *node)
{
  cache_node_t *older, *newer;
  
  debug("cache_update_impl(cache=0x%x, node=0x%x)\n", cache, node);
  
  older = node->older;
  newer = node->newer;
  
  /* update least-recently-used */
  if (node == cache->lru && cache->lru->newer) {
    cache->lru = cache->lru->newer;
  }
  
  /* update most-recently-used */
  if (node != cache->mru) {
    if (older) {
      older->newer = newer;
    }
    if (newer) {
      newer->older = older;
    }
    node->older       = cache->mru;
    node->newer       = NULL;
    cache->mru->newer = node;
    cache->mru        = node;
  }
}

void
cache_insert_impl(cache_t *cache, void const *key)
{
  cache_node_t *node, *previous;
  size_t       hash;
  
  debug("cache_insert_impl(cache=0x%x, key=0x%x)\n", cache, key);
  
  /* if we will be violating the size restrictions, then adjust */
  if (cache->size >= cache->max_size) {
    previous = cache->lru;
    if (cache->lru->newer) {
      cache->lru        = cache->lru->newer;
      cache->lru->older = NULL;
    }
    cache_remove(cache, previous->key);
  }
  
  hash = cache_bucket(cache, key);
  node = cache->nodes[hash];
  
  debug("cache_insert_impl: hash=%d ~> root-node=0x%x\n", hash, node);
  
  /* search within the bucket */
  while (node) {
    if (0 == cache->comparator(node->key, key)) {
      /* found */
      debug("cache_insert_impl: hash=%d => node=0x%x\n", hash, node);
      return;
    }
    node = node->next;
  }
  
  /* create new data node */
  node        = MALLOC(cache_node_t);
  node->key   = cache->duplicator(key);
  node->next  = cache->nodes[hash];
  node->older = NULL;
  node->newer = NULL;
  
  /* the cache just got bigger */
  cache->nodes[hash] = node;
  cache->size++;
  
  /* update most-recently-used */
  if (cache->mru) {
    cache->mru->newer = node;
  }
  cache->mru = node;
  
  /* update least-recently-used */
  if (!cache->lru) {
    cache->lru = cache->mru;
  } else if (!cache->lru->newer) {
    cache->lru->newer = cache->mru;
  }
  
  debug("cache_insert_impl: hash=%d => node=0x%x\n", hash, node);
}

cache_node_t*
cache_find(cache_t *cache, void const *key)
{
  cache_node_t *node;
  size_t       hash;
  
  debug("cache_find(cache=0x%x, key=0x%x)\n", cache, key);
  
  hash = cache_bucket(cache, key);
  node = cache->nodes[hash];
  
  debug("cache_find: hash=%d ~> node=0x%x\n", hash, node);
  cache_debug(cache);
  
  while (node) {
    debug("cache_find: comparing node->key=0x%x to key=0x%x\n", node->key, key);
    if (0 == cache->comparator(node->key, key)) {
      /* found */
      debug("cache_find: hash=%d => node=0x%x, found!\n", hash, node);
      return node;
    }
    node = node->next;
  }
  
  return NULL;
}

void
cache_miss(cache_t *cache, void const *key, cache_operation::type_t access)
{
  size_t hash, size;
  bool   existed, conflict;
  
  debug("cache_miss(cache=0x%x, key=0x%x, access='%s')\n", cache, key, cache_operation_to_string(access));
  
  existed = hash_table_insert(cache->addresses, key, NULL);
  debug("cache_miss: key=0x%x, existed='%s'\n", key, bool_to_string(existed));
  //hash_table_debug(cache->addresses);
  
  hash     = cache_bucket(cache, key);
  size     = cache_bucket_size(cache, hash);
  conflict = (!existed && size > 0);
  debug("cache_miss: hash=%d, (bucket) size=%d, conflict='%s'\n", hash, size, bool_to_string(conflict));
  
  if (cache_operation::read == access) {
    if (existed) {
      if (conflict) {
	cache->statistics.conflict_read_misses++;
      } else {
	cache->statistics.read_misses++;
      }
    } else {
      cache->statistics.compulsory_read_misses++;
    }
  } else {
    if (existed) {
      if (conflict) {
	cache->statistics.conflict_write_misses++;
      } else {
	cache->statistics.write_misses++;
      }
    } else {
      cache->statistics.compulsory_write_misses++;
    }
  }
}

void
cache_hit(cache_t *cache, void const *key, cache_operation::type_t access)
{
  debug("cache_hit(cache=0x%x, access='%s')\n", cache, cache_operation_to_string(access));
  
  switch (access) {
  case cache_operation::read:
    cache->statistics.read_hits++;
    break;
  case cache_operation::write:
    cache->statistics.write_hits++;
    break;
  }
}

void
cache_access(cache_t *cache, void const *key, cache_operation::type_t access)
{
  cache_node_t *node;
  
  debug("cache_access(cache=0x%x, key=0x%x, access='%s')\n", cache, key, cache_operation_to_string(access));
  
  if (NULL == (node = cache_find(cache, key))) {
    cache_insert_impl(cache, key);
    cache_miss(cache, key, access);
  } else {
    cache_update_impl(cache, node);
    cache_hit(cache, key, access);
  }
}

void
cache_supported(cache_t *cache)
{
  if (!cache) {
    die("We received a null cache.\n");
  }
  
  if (!is_power_of_two(cache->max_size)) {
    die("Cache size is not a power of two (size=%d).\n", cache->max_size);
  }

  if (!is_power_of_two(cache->max_size/cache->line_size)) {
    die("Cache set is not a power of two (%d).\n", cache->size/cache->line_size);
  }
  
  if (!is_power_of_two(cache->line_size)) {
    die("Cache line size is not a power of two.\n");
  }
  
  if (0 != (cache->max_size % cache->line_size)) {
    die("Cache size is not evenly divisible by line size.\n");
  }
  
  if (cache->line_size < MINIMUM_CACHE_LINE_SIZE) {
    die("Cache line size is too small.\n");
  }
  
  if (cache->max_size <= cache->line_size) {
    die("Cache size is less than or equal to line size.\n");
  }
}

char const*
cache_operation_to_string(cache_operation::type_t access)
{
  static char const* read_string  = "read";
  static char const* write_string = "write";
  return (cache_operation::read == access ? read_string : write_string);
}

void
cache_print_statistics(cache_t *cache)
{
  ulong              accesses, compulsory, conflict, hits, misses;
  ulong              ideal_accesses, ideal_misses;
  cache_statistics_t *statistics;
  
  debug("cache_print_statistics(cache=0x%x)\n", cache);
  
  statistics     = &cache->statistics;
  hits           = statistics->read_hits + statistics->write_hits;
  compulsory     = statistics->compulsory_read_misses + statistics->compulsory_write_misses;
  conflict       = statistics->conflict_read_misses + statistics->conflict_write_misses;
  misses         = statistics->read_misses + statistics->write_misses + compulsory + conflict;
  accesses       = hits + misses;
  ideal_accesses = accesses - compulsory;
  ideal_misses   = misses - compulsory;
  
  message("Cache Profile:\n");
  message("accesses:                %d\n", accesses);
  message("read hits:               %d\n", statistics->read_hits);
  message("write hits:              %d\n", statistics->write_hits);
  message("total hits:              %d\n", hits);
  message("read misses:             %d\n", statistics->read_misses);
  message("write misses:            %d\n", statistics->write_misses);
  message("compulsory read misses:  %d\n", statistics->compulsory_read_misses);
  message("compulsory write misses: %d\n", statistics->compulsory_write_misses);
  message("total compulsory misses: %d\n", compulsory);
  message("conflict read misses:    %d\n", statistics->conflict_read_misses);
  message("conflict write misses:   %d\n", statistics->conflict_write_misses);
  message("total conflict misses:   %d\n", conflict);
  message("total misses:            %d\n", misses);
  message("ideal misses:            %d\n", ideal_misses);
  message("hit rate:                %2.4f\n", 1.0 - (double) misses / (double) accesses);
  message("miss rate:               %2.4f\n", (double) misses / (double) accesses);
  message("ideal hit rate:          %2.4f\n", 1.0 - (double) ideal_misses / (double) ideal_accesses);
  message("ideal miss rate:         %2.4f\n", (double) ideal_misses / (double) ideal_accesses);
  
#if 0
  message("replace:   %d\n", replacements);
  statistics->read_hits               = 0;
  statistics->read_misses             = 0;
  statistics->compulsory_read_misses  = 0;
  statistics->write_hits              = 0;
  statistics->write_misses            = 0;
  statistics->compulsory_write_misses = 0;
#endif
}

void
cache_debug(cache_t *cache)
{
  uint         i;
  cache_node_t *node, *next;
  
  if (verbose) {
    message("Cache Contents (%d/%d):\n", cache->size, cache->max_size);
    for (i = 0; i < cache->max_size; ++i) {
      node = cache->nodes[i];
      if (node) {
	message("%4d: ", i);
	for (; node; node = next) {
	  next = node->next;
	  message("0x%x ", node->key);
	}
	message("\n");
      }
    }
  }
}
