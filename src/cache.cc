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

extern bool human_readable;
extern uint cache_size;
extern uint cache_line_size;
extern bool simulate;
extern bool verbose;
size_t      hash_shift, tag_shift;

static size_t
cache_line_hash(void const *address)
{
  register size_t key;
  
  key = (size_t) address;
  return ((key >> hash_shift) << hash_shift);
}

static size_t
cache_line_compare(void const *a, void const *b)
{
  register size_t x, y;
  
  x = cache_line_hash(a);
  y = cache_line_hash(b);
  
  if (x < y) {
    return -1;
  } else if (x > y) {
    return 1;
  }
  return 0;
}

cache_t*
cache_malloc(size_t cache_size, size_t cache_line_size)
{
  uint               i, lines;
  cache_t            *cache;
  hash_table_t       *addresses;
  cache_statistics_t *statistics;
  
  debug("cache_malloc(cache_size=%d, cache_line_size=%d)\n", cache_size, cache_line_size);
  
  lines           = cache_size / cache_line_size;
  cache_size      *= 64;
  cache_line_size *= 64;
  hash_shift      = log2(cache_line_size);
  tag_shift       = log2(cache_size);
  
  debug("cache_malloc: lines=%d, hash_shift=%d, tag_shift=%d\n", lines, hash_shift, tag_shift);
  
  cache                  = MALLOC(cache_t);
  cache->nodes           = MALLOC_N(cache_node_t*, lines);
  cache->cache_size      = cache_size;
  cache->cache_line_size = cache_line_size;
  cache->lines           = lines;
  cache->entries         = 0;
  cache->ticks           = 0;
  cache->mru             = NULL;
  cache->lru             = NULL;
  
  addresses              = hash_table_malloc(lines);
  addresses->hasher      = &cache_line_hash;
  addresses->comparator  = &cache_line_compare;
  cache->addresses       = addresses;
  
  for (i = 0; i < lines; ++i) {
    cache->nodes[i] = NULL;
  }
  
  for (i = 'A'; i < 'Z'; ++i) {
    cache->has_statistics[i-'A']        = false;
    statistics                          = &cache->statistics[i-'A'];
    statistics->read_hits               = 0;
    statistics->read_misses             = 0;
    statistics->compulsory_read_misses  = 0;
    statistics->conflict_read_misses    = 0;
    statistics->write_hits              = 0;
    statistics->write_misses            = 0;
    statistics->compulsory_write_misses = 0;
    statistics->conflict_write_misses   = 0;
  }
  
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
      /* cache->freer(cnode->key); */
      safe_free(cnode);
      cnode = cnext;
    }
  }
  
  for (i = 0; i < cache->addresses->max_size; ++i) {
    hnode = cache->addresses->nodes[i];
    while (hnode) {
      hnext = hnode->next;
      dnode = (cache_line_lifetime_t*) hnode->data;
      safe_free(dnode->name);
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

void
cache_line_lifetime_start(cache_t *cache, void const *key, char const *name)
{
  hash_table_node_t     *node;
  cache_line_lifetime_t *data;
  size_t                hash;
  
  debug("cache_line_lifetime_start(cache=0x%x, key=0x%x, name='%s')\n", cache, key, name);
  
  data        = MALLOC(cache_line_lifetime_t);
  data->name  = strdup(name);
  data->start = cache->ticks;
  data->end   = 0;
  data->next  = NULL;
  
  if (NULL == (node = hash_table_find(cache->addresses, key))) {
    node = hash_table_insert(cache->addresses, key, data);
  } else {
    data->next = (cache_line_lifetime_t*) node->data;
    hash_table_update(cache->addresses, node, data);
  }
  
  hash = compute_cache_line_hash(cache, key);
  trace("added:\t\t%s (key=0x%x, hash=%d)\n", data->name, key, hash);
}

void
cache_line_lifetime_end(cache_t *cache, void const *key)
{
  hash_table_node_t     *node;
  cache_line_lifetime_t *data;
  size_t                hash;
  
  debug("cache_line_lifetime_end(cache=0x%x, key=0x%x)\n", cache, key);
  
  node      = hash_table_find(cache->addresses, key);
  data      = (cache_line_lifetime_t*) node->data;
  data->end = cache->ticks;
  
  hash = compute_cache_line_hash(cache, key);
  trace("evicted:\t%s (key=0x%x, hash=%d, lifetime=%d)\n", data->name, key, hash, data->end - data->start);
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
      /* cache->freer(node->key); */
      safe_free(node->name);
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
cache_update(cache_t *cache, cache_node_t *node, char const *name)
{
  cache_node_t *older, *newer;
  
  debug("cache_update(cache=0x%x, node=0x%x, name='%s')\n", cache, node, name);
  
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
cache_insert(cache_t *cache, void const *key, char const *name)
{
  cache_node_t *node, *previous;
  size_t       hash;
  
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
  
  /* create new data node */
  hash        = compute_cache_line_hash(cache, key);
  node        = MALLOC(cache_node_t);
  node->key   = (void*) key; /* cache->duplicator(key); */
  node->tag   = compute_cache_line_tag(cache, key);
  node->name  = strdup(name);
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
  
  /* begin the cache-line's life */
  cache_line_lifetime_start(cache, key, name);
  
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
cache_miss(cache_t *cache, void const *key, cache_operation::type_t access, char const *name)
{
  size_t             hash;
  bool               existed, conflict;
  cache_statistics_t *statistics;
  
  debug("cache_miss(cache=0x%x, key=0x%x, access='%s')\n", cache, key, cache_operation_to_string(access));
  
  hash       = compute_cache_line_hash(cache, key);
  statistics = &cache->statistics[name[0]-'A'];
  
  cache->has_statistics[name[0]-'A'] = true;
  
  trace("miss:\t\t%s (key=0x%x, hash=%d)\n", name, key, hash);
  
  existed  = (NULL != hash_table_find(cache->addresses, key));
  conflict = (NULL != cache->nodes[hash]);
  
  debug("cache_miss: key=0x%x, name='%s', existed='%s', hash=%d, conflict='%s'\n",
	key, name, bool_to_string(existed), hash, bool_to_string(conflict));
  
  if (cache_operation::read == access) {
    if (existed) {
      if (conflict) {
	statistics->conflict_read_misses++;
      } else {
	statistics->read_misses++;
      }
    } else {
      statistics->compulsory_read_misses++;
    }
  } else {
    if (existed) {
      if (conflict) {
	statistics->conflict_write_misses++;
      } else {
	statistics->write_misses++;
      }
    } else {
      statistics->compulsory_write_misses++;
    }
  }
}

void
cache_hit(cache_t *cache, void const *key, cache_operation::type_t access, char const *name)
{
  size_t             hash;
  cache_statistics_t *statistics;
    
  debug("cache_hit(cache=0x%x, access='%s')\n", cache, cache_operation_to_string(access));
  
  hash       = compute_cache_line_hash(cache, key);
  statistics = &cache->statistics[name[0]-'A'];
  
  cache->has_statistics[name[0]-'A'] = true;
  
  trace("hit:\t\t%s (key=0x%x, hash=%d)\n", name, key, hash);
  
  switch (access) {
  case cache_operation::read:
    statistics->read_hits++;
    break;
  case cache_operation::write:
    statistics->write_hits++;
    break;
  }
}

void
cache_access(cache_t *cache, void const *key, cache_operation::type_t access, char const *format, ...)
{
  va_list      args;
  char         name[100];
  cache_node_t *node;
  
  if (!simulate) {
    return;
  }
  
  debug("cache_access(cache=0x%x, key=0x%x, access='%s')\n", cache, key, cache_operation_to_string(access));
  
  va_start(args, format);
  vsprintf(name, format, args);
  va_end(args);
  
  debug("cache_access: name='%s'\n", name);
  
  if (NULL == (node = cache_find(cache, key))) {
    cache_miss(cache, key, access, name);
    cache_insert(cache, key, name);
  } else {
    cache_hit(cache, key, access, name);
    cache_update(cache, node, name);
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
  hash_table_node_t     *node;
  cache_line_lifetime_t *data;
    
  debug("cache_print_lifetimes(cache=0x%x)\n", cache);
  
  message("Cache-Line Lifetimes: (out of %d)\n", cache->ticks);
  for (i = 0; i < cache->addresses->max_size; ++i) {
    node = cache->addresses->nodes[i];
    while (node) {
      data = (cache_line_lifetime_t*) node->data;
      message("%5d:0x%0.8x: %15s: ", i, node->key, data->name);
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
cache_print_human_readable_statistics(cache_t *cache, cache_statistics_t *statistics, char const *name)
{
  ulong              accesses, compulsory, conflict, hits, misses;
  ulong              nontrivial_accesses, nontrivial_misses;
  
  debug("cache_print_human_readable_statistics(statistics=0x%x, name='%s')\n", statistics, name);
  
  hits                = statistics->read_hits + statistics->write_hits;
  compulsory          = statistics->compulsory_read_misses + statistics->compulsory_write_misses;
  conflict            = statistics->conflict_read_misses + statistics->conflict_write_misses;
  misses              = statistics->read_misses + statistics->write_misses + compulsory + conflict;
  accesses            = hits + misses;
  nontrivial_accesses = accesses - compulsory;
  nontrivial_misses   = misses - compulsory;
  
  message("Statistics for %s:\n", name);
  message("cache size:                %d\n", cache->cache_size);
  message("cache-line size:           %d\n", cache->cache_line_size);
  message("number of cache-lines:     %d\n", cache->lines);
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
}

void
cache_print_raw_statistics(cache_t *cache, cache_statistics_t *statistics, char const *name)
{
  ulong              accesses, compulsory, conflict, hits, misses;
  ulong              nontrivial_accesses, nontrivial_misses;
  
  debug("cache_print_raw_statistics(statistics=0x%x, name='%s')\n", statistics, name);
  
  hits                = statistics->read_hits + statistics->write_hits;
  compulsory          = statistics->compulsory_read_misses + statistics->compulsory_write_misses;
  conflict            = statistics->conflict_read_misses + statistics->conflict_write_misses;
  misses              = statistics->read_misses + statistics->write_misses + compulsory + conflict;
  accesses            = hits + misses;
  nontrivial_accesses = accesses - compulsory;
  nontrivial_misses   = misses - compulsory;
  
  message("%s\t", name);
  message("%d\t", cache->cache_size);
  message("%d\t", cache->cache_line_size);
  message("%d\t", cache->lines);
  message("%d\t", accesses);
  message("%d\t", statistics->read_hits);
  message("%d\t", statistics->write_hits);
  message("%d\t", hits);
  message("%d\t", statistics->read_misses);
  message("%d\t", statistics->write_misses);
  message("%d\t", statistics->compulsory_read_misses);
  message("%d\t", statistics->compulsory_write_misses);
  message("%d\t", compulsory);
  message("%d\t", statistics->conflict_read_misses);
  message("%d\t", statistics->conflict_write_misses);
  message("%d\t", conflict);
  message("%d\t", misses);
  message("%d\t", nontrivial_misses);
  message("%2.4f\t", (double) nontrivial_misses / (double) misses);
  message("%2.4f\t", 1.0 - (double) misses / (double) accesses);
  message("%2.4f\t", (double) misses / (double) accesses);
  message("%2.4f\t", 1.0 - (double) nontrivial_misses / (double) nontrivial_accesses);
  message("%2.4f\n", (double) nontrivial_misses / (double) nontrivial_accesses);
}

void
cache_print_statistics_header()
{
  message("# name\tcache size\tcache-line size\tnumber of cache-lines\taccesses\tread hits\twrite hits\ttotal hits\tread misses\twrite misses\tcompulsory read misses\tcompulsory write misses\ttotal compulsory misses\tconflict read misses\tconflict write misses\ttotal conflict misses\ttotal misses\tnon-trivial misses\tnon-trivial to miss ratio\thit rate\tmiss rate\tnon-trivial hit rate\tnon-trivial miss rate\n");
}

void
cache_print_statistics(cache_t *cache, cache_statistics_t *statistics, char const *name)
{
  if (human_readable) {
    cache_print_human_readable_statistics(cache, statistics, name);
  } else {
    cache_print_raw_statistics(cache, statistics, name);
  }
}

void
add_statistics(cache_statistics_t *destination, cache_statistics_t *source)
{
  debug("add_statistics(destination=0x%x, source=0x%x)\n", destination, source);
  
  destination->read_hits               += source->read_hits;
  destination->write_hits              += source->write_hits;
  destination->read_misses             += source->read_misses;
  destination->write_misses            += source->write_misses;
  destination->compulsory_read_misses  += source->compulsory_read_misses;
  destination->compulsory_write_misses += source->compulsory_write_misses;
  destination->conflict_read_misses    += source->conflict_read_misses;
  destination->conflict_write_misses   += source->conflict_write_misses;
}

void
cache_print_profile(cache_t *cache)
{
  uint               i;
  char               name[10];
  cache_statistics_t totals, *statistics;
  
  totals.read_hits               = 0;
  totals.write_hits              = 0;
  totals.read_misses             = 0;
  totals.write_misses            = 0;
  totals.compulsory_read_misses  = 0;
  totals.compulsory_write_misses = 0;
  totals.conflict_read_misses    = 0;
  totals.conflict_write_misses   = 0;
  
  if (!human_readable) {
    cache_print_statistics_header();
  }
  
  for (i = 'A'; i < 'Z'; ++i) {
    if (cache->has_statistics[i-'A']) {
      statistics = &cache->statistics[i-'A'];
      add_statistics(&totals, statistics);
      sprintf(name, "%c", i);
      cache_print_statistics(cache, statistics, name);
      message("\n");
    }
  }
  
  sprintf(name, "total");
  cache_print_statistics(cache, &totals, name);
  
#if 0
  message("\n");
  cache_print_lifetimes(cache);
#endif
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
