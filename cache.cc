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

/*
  Ok, so the whole idea is to try and simulate an "ideal" cache here.
  We use a least-recently-used (LRU) eviction policy, as it has been
  shown to perform closest to the clairvoyant replacement policy
  (OPT).
  
  We use a fusion of a hash table and a linked list.  The hash table
  allows for fast lookups.  The linked list maintains the access
  order. This was probably overkill for what we are doing, but
  because we will be running 100s, even 1000s, of iterations, it has
  the nice effect of not destroying my machine and exhausting
  patience.
  
  
*/

#include "address.h"
#include "cache.h"
#include "error.h"
#include "memory.h"
#include "utility.h"
#include <stdio.h>
#include <string.h>

#define FULL_CACHE_DEBUG_WOULD_BE_TOO_VERBOSE 10

extern bool verbose;
extern bool simulate;
size_t      hash_shift, tag_shift;
  
size_t cache_key_hash(void const *address);
size_t cache_key_tag(void const *address);
size_t cache_key_compare(void const *a, void const *b);
void*  cache_key_duplicate(void const *address);
void   cache_key_free(void *address);

cache_t*
cache_malloc(size_t cache_size, size_t cache_line_size)
{
  uint    i, lines;
  cache_t *cache;
  
  debug("cache_malloc(cache_size=%d, cache_line_size=%d)\n", cache_size, cache_line_size);
  
  lines      = cache_size / cache_line_size;
  hash_shift = log2(cache_line_size);
  tag_shift  = log2(cache_line_size) + log2(cache_size);
  
  debug("cache_malloc: lines=%d, hash_shift=%d, tag_shift=%d\n", lines, hash_shift, tag_shift);
  
  cache                  = MALLOC(cache_t);
  cache->nodes           = MALLOC_N(cache_node_t*, lines);
  cache->addresses       = hash_table_malloc(cache_size);
  cache->cache_size      = cache_size;
  cache->cache_line_size = cache_line_size;
  cache->lines           = lines;
  cache->entries         = 0;
  cache->ticks           = 0;
  cache->mru             = NULL;
  cache->lru             = NULL;
  
  for (i = 0; i < lines; ++i) {
    cache->nodes[i] = NULL;
  }
  
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
  size_t                i;
  cache_node_t          *cnode, *cnext;
  hash_table_node_t     *hnode, *hnext;
  cache_line_lifetime_t *dnode, *dnext;

  debug("cache_free(cache=0x%x)\n", cache);
  
  for (i = 0; i < cache->lines; ++i) {
    cnode = cache->nodes[i];
    while (cnode) {
      cnext = cnode->next;
      cache_key_free(cnode->key);
      safe_free(cnode);
      cnode = cnext;
    }
  }
  
  for (i = 0; i < cache->addresses->max_size; ++i) {
    hnode = cache->addresses->nodes[i];
    while (hnode) {
      hnext = hnode->next;
      dnode = (cache_line_lifetime_t*) hnode->data;
      while (dnode) {
	dnext = dnode->next;
	safe_free(dnode);
	dnode = dnext;
      }
      hnode = hnext;
    }
  }
  
  hash_table_free(cache->addresses);
  safe_free(cache->nodes);
  safe_free(cache);
}

/* http://faculty.cs.niu.edu/~berezin/463/lec/06memory/cachenote1.html 
*/
size_t
compute_cache_line_hash(cache_t *cache, void const *address)
{
  register size_t key;
  
  key = (size_t) address;
  return (key % cache->cache_size) / cache->cache_line_size;
}

size_t
compute_cache_line_tag(cache_t *cache, void const *address)
{
  register size_t key;
  
  key = (size_t) address;
  return key / cache->cache_size;
}

size_t
compute_cache_line_size(cache_t *cache, size_t hash)
{
  size_t       size;
  cache_node_t *node;
  
  size = 0;
  node = cache->nodes[hash];
  
  while (node) {
    size++;
    node = node->next;
  }
  
  return size;
}

bool
cache_line_lifetime_start(cache_t *cache, void const *key)
{
  bool                  existed;
  hash_table_node_t     *node;
  cache_line_lifetime_t *data;
  
  debug("cache_line_lifetime_start(cache=0x%x, key=0x%x)\n", cache, key);
  
  data        = MALLOC(cache_line_lifetime_t);
  data->start = cache->ticks;
  data->end   = 0;
  data->next  = NULL;
  existed     = false;
  
  if (NULL == (node = hash_table_find(cache->addresses, key))) {
    node = hash_table_insert(cache->addresses, key, data);
  } else {
    existed    = true;
    data->next = (cache_line_lifetime_t*) node->data;
    hash_table_update(cache->addresses, node, data);
  }

  return existed;
}

void
cache_line_lifetime_end(cache_t *cache, void const *key)
{
  hash_table_node_t     *node;
  cache_line_lifetime_t *data;
  
  debug("cache_line_lifetime_start(cache=0x%x, key=0x%x)\n", cache, key);
  
  node      = hash_table_find(cache->addresses, key);
  data      = (cache_line_lifetime_t*) node->data;
  data->end = cache->ticks;
}

void
cache_remove(cache_t *cache, void const *key)
{
  cache_node_t *node, *previous;
  size_t       hash, tag;
  
  debug("cache_remove(cache=0x%x, key=0x%x)\n", cache, key);
  
  /* we are evicting in new line, so end the lifetime */
  cache_line_lifetime_end(cache, key);
  
  hash     = compute_cache_line_hash(cache, key);
  tag      = compute_cache_line_tag(cache, key);
  node     = cache->nodes[hash];
  previous = NULL;
  
  debug("cache_remove: hash=%d, tag=%d ~> node=0x%x\n", hash, tag, node);
  
  while (node) {
    if (tag == node->tag) {
      /* found */
      debug("cache_remove: hash=%d => node=0x%x, tag=%d\n", hash, node, node->tag);
      if (previous) {
	if (previous != node->next) {
	  previous->next = node->next;
	} else {
	  previous->next = NULL;
	}
      } else {
	cache->nodes[hash] = node->next;
      }
      cache_key_free(node->key);
      safe_free(node);
      cache->entries--;
      return;
    }
    previous = node;
    node     = node->next;
  }
  
  die("Failed to remove key 0x%x; it was not found.\n", key);
}

void
cache_update(cache_t *cache, cache_node_t *node)
{
  cache_node_t *older, *newer;
  
  debug("cache_update(cache=0x%x, node=0x%x)\n", cache, node);
  
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
cache_insert(cache_t *cache, void const *key)
{
  cache_node_t *node, *previous;
  size_t       hash, tag;
  
  debug("cache_insert(cache=0x%x, key=0x%x)\n", cache, key);
  
  /* if we will be violating the size restrictions, then adjust */
  if (cache->entries >= cache->lines) {
    previous = cache->lru;
    if (cache->lru->newer) {
      cache->lru        = cache->lru->newer;
      cache->lru->older = NULL;
    }
    cache_remove(cache, previous->key);
  }
  
  hash = compute_cache_line_hash(cache, key);
  tag  = compute_cache_line_tag(cache, key);
  node = cache->nodes[hash];
  
  debug("cache_insert: hash=%d, tag=%d ~> node=0x%x\n", hash, tag, node);
  
  /* search within the bucket */
  while (node) {
    if (tag == node->tag) {
      /* found */
      debug("cache_insert: hash=%d => node=0x%x\n", hash, node);
      return;
    }
    node = node->next;
  }
  
  /* create new data node */
  node        = MALLOC(cache_node_t);
  node->key   = cache_key_duplicate(key);
  node->tag   = tag;
  node->next  = cache->nodes[hash];
  node->older = NULL;
  node->newer = NULL;
  
  /* the cache just got bigger */
  cache->nodes[hash] = node;
  cache->entries++;
  
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
  
  debug("cache_insert: hash=%d => node=0x%x\n", hash, node);
}

cache_node_t*
cache_find(cache_t *cache, void const *key)
{
  cache_node_t *node;
  size_t       hash, tag;
  
  debug("cache_find(cache=0x%x, key=0x%x)\n", cache, key);
  
  hash = compute_cache_line_hash(cache, key);
  tag  = compute_cache_line_tag(cache, key);
  node = cache->nodes[hash];
  
  debug("cache_find: hash=%d, tag=%d ~> node=0x%x\n", hash, tag, node);
  cache_debug(cache);
  
  while (node) {
    debug("cache_find: comparing node->tag=%d to tag=%d, key=0x%x\n", node->tag, tag, node->key);
    if (tag == node->tag) {
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
  size_t                hash, tag;
  bool                  existed, conflict;
  
  debug("cache_miss(cache=0x%x, key=0x%x, access='%s')\n", cache, key, cache_operation_to_string(access));
  
  existed  = cache_line_lifetime_start(cache, key);
  hash     = compute_cache_line_hash(cache, key);
  tag      = compute_cache_line_tag(cache, key);
  conflict = (NULL != cache->nodes[hash]);
  
  debug("cache_miss: key=0x%x, existed='%s', hash=%d, tag=%d, conflict='%s'\n",
	key, bool_to_string(existed), hash, tag, bool_to_string(conflict));
  
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
  
  if (!simulate) {
    return;
  }
  
  debug("cache_access(cache=0x%x, key=0x%x, access='%s')\n", cache, key, cache_operation_to_string(access));
  
  if (NULL == (node = cache_find(cache, key))) {
    cache_miss(cache, key, access);
    cache_insert(cache, key);
  } else {
    cache_hit(cache, key, access);
    cache_update(cache, node);
  }
  
  cache->ticks++;
}

void
cache_supported(cache_t *cache)
{
  if (!cache) {
    die("We received a null cache.\n");
  }
  
  if (!is_power_of_two(cache->cache_size)) {
    die("Cache size is not a power of two (size=%d).\n", cache->cache_size);
  }
  
  if (!is_power_of_two(cache->lines)) {
    die("Cache set is not a power of two (%d).\n", cache->lines);
  }
  
  if (!is_power_of_two(cache->cache_line_size)) {
    die("Cache line size is not a power of two.\n");
  }
  
  if (0 != (cache->cache_size % cache->cache_line_size)) {
    die("Cache size is not evenly divisible by line size.\n");
  }
  
  if (cache->cache_line_size < MINIMUM_CACHE_LINE_SIZE) {
    die("Cache line size is too small.\n");
  }
  
  if (cache->cache_size <= cache->cache_line_size) {
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
cache_print_lifetimes(cache_t *cache)
{
  uint                  i;
  hash_table_t          *addresses;
  hash_table_node_t     *node;
  cache_line_lifetime_t *data;
  
  debug("cache_print_lifetimes(cache=0x%x)\n", cache);
  
  addresses = cache->addresses;
  
  message("Cache-Line Lifetimes: (out of %d)\n", cache->ticks);
  for (i = 0; i < addresses->max_size; ++i) {
    node = addresses->nodes[i];
    while (node) {
      message("%4d/0x%x: ", i, node->key);
      data = (cache_line_lifetime_t*) node->data;
      while (data) {
	if (0 == data->end) {
	  data->end = cache->ticks;
	}
	message("%4d ", data->end - data->start);
	data = data->next;
      }
      message("\n");
      node = node->next;
    }
  }
}

void
cache_print_profile(cache_t *cache)
{
  ulong              accesses, compulsory, conflict, hits, misses;
  ulong              nontrivial_accesses, nontrivial_misses;
  cache_statistics_t *statistics;
  
  debug("cache_print_profile(cache=0x%x)\n", cache);
  
  statistics          = &cache->statistics;
  hits                = statistics->read_hits + statistics->write_hits;
  compulsory          = statistics->compulsory_read_misses + statistics->compulsory_write_misses;
  conflict            = statistics->conflict_read_misses + statistics->conflict_write_misses;
  misses              = statistics->read_misses + statistics->write_misses + compulsory + conflict;
  accesses            = hits + misses;
  nontrivial_accesses = accesses - compulsory;
  nontrivial_misses   = misses - compulsory;
  
  message("Cache Profile:\n");
  message("accesses:                  %d\n", accesses);
  message("read hits:                 %d\n", statistics->read_hits);
  message("write hits:                %d\n", statistics->write_hits);
  message("total hits:                %d\n", hits);
  message("read misses:               %d\n", statistics->read_misses);
  message("write misses:              %d\n", statistics->write_misses);
  message("compulsory read misses:    %d\n", statistics->compulsory_read_misses);
  message("compulsory write misses:   %d\n", statistics->compulsory_write_misses);
  message("total compulsory misses:   %d\n", compulsory);
  message("conflict read misses:      %d\n", statistics->conflict_read_misses);
  message("conflict write misses:     %d\n", statistics->conflict_write_misses);
  message("total conflict misses:     %d\n", conflict);
  message("total misses:              %d\n", misses);
  message("non-trivial misses:        %d\n", nontrivial_misses);
  message("non-trivial to miss ratio: %2.4f\n", (double) nontrivial_misses / (double) misses);
  message("hit rate:                  %2.4f\n", 1.0 - (double) misses / (double) accesses);
  message("miss rate:                 %2.4f\n", (double) misses / (double) accesses);
  message("non-trivial hit rate:      %2.4f\n", 1.0 - (double) nontrivial_misses / (double) nontrivial_accesses);
  message("non-trivial miss rate:     %2.4f\n", (double) nontrivial_misses / (double) nontrivial_accesses);
  
  message("\n");
  cache_print_lifetimes(cache);
}

void
cache_debug(cache_t *cache)
{
  uint         i;
  cache_node_t *node;
  
  if (!simulate) {
    return;
  }
  
  if (verbose) {
    message("Cache Contents (%d/%d):\n", cache->entries, cache->lines);
    for (i = 0; i < cache->lines; ++i) {
      node = cache->nodes[i];
      if (node || cache->lines < FULL_CACHE_DEBUG_WOULD_BE_TOO_VERBOSE) {
	message("%4d: ", i);
	while (node) {
	  message("0x%x ", node->key);
	  node = node->next;
	}
	message("\n");
      }
    }
  }
}

/* Source: http://www.concentric.net/~ttwang/tech/addrhash.htm
   
   Fibonacci hash function.  The multiplier is the nearest prime to
   (2^32 times (√5 - 1)/2). (2654435761 is the golden ratio of 2^32.)
   See Knuth §6.4: volume 3, 3rd ed, p518.x */
size_t
cache_key_hash(void const *address)
{
  register size_t key;
  
  key = (size_t) address;
  return (key >> hash_shift);
}

size_t
cache_key_tag(void const *address)
{
  register size_t key;
  
  key = (size_t) address;
  return (key >> tag_shift);
}

size_t
cache_key_compare(void const *a, void const *b)
{
  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  }
  return 0;
}

void*
cache_key_duplicate(void const *address)
{
  return (void*) address;
}

void
cache_key_free(void *address)
{
  /* no-op */
}
