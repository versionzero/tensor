/*
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
#include <assert.h>
#include <stdio.h>
#include <string.h>

extern bool verbose;

hash_table_t*
hash_table_malloc(size_t max_size)
{
  uint         i;
  hash_table_t *table;
  
  debug("hash_table_malloc(max_size=%d)\n", max_size);
  
  table             = MALLOC(hash_table_t);
  table->nodes      = MALLOC_N(hash_table_node_t*, max_size);
  table->max_size   = max_size;
  table->size       = 0;
  table->hasher     = &memory_address_hash;
  table->comparator = &memory_address_compare;
  table->duplicator = &memory_address_duplicate;
  table->freer      = &memory_address_free;
  
  for (i = 0; i < table->max_size; ++i) {
    table->nodes[i] = NULL;
  }
  
  debug("hash_table_malloc: hasher=0x%x\n",     table->hasher);
  debug("hash_table_malloc: comparator=0x%x\n", table->comparator);
  debug("hash_table_malloc: duplicator=0x%x\n", table->duplicator);
  debug("hash_table_malloc: freer=0x%x\n",      table->freer);
  debug("hash_table_malloc: table=0x%x\n",      table);
  
  return table;
}

void
hash_table_free(hash_table_t *table)
{
  size_t            i;
  hash_table_node_t *node, *next;
	
  debug("hash_table_free(table=0x%x)\n", table);
  
  for (i = 0; i < table->max_size; ++i) {
    node = table->nodes[i];
    while (node) {
      next = node->next;
      table->freer(node->key);
      safe_free(node);
      node = next;
    }
  }
  
  safe_free(table->nodes);
  safe_free(table);
}

size_t
hash_table_bucket(hash_table_t *table, void const *key)
{
  assert(NULL != table->hasher);
  
  return table->hasher(key) % table->max_size;
}

void
hash_table_remove(hash_table_t *table, void const *key)
{
  hash_table_node_t *node, *previous;
  size_t            hash;
  
  debug("hash_table_remove(table=0x%x, key=0x%x)\n", table, key);
  
  hash     = hash_table_bucket(table, key);
  node     = table->nodes[hash];
  previous = NULL;
  
  debug("hash_table_remove: hash=%d ~> root-node=0x%x\n", hash, node);
  
  while (node) {
    if (0 == table->comparator(node->key, key)) {
      /* found */
      debug("hash_table_remove: hash=%d => node=0x%x\n", hash, node);
      if (previous) {
	previous->next = node->next;
      } else {
	table->nodes[hash] = node->next;
      }
      table->freer(node->key);
      safe_free(node);
      table->size--;
      return;
    }
    previous = node;
    node     = node->next;
  }
  
  die("Failed to remove key 0x%x; it was not found.\n", key);
}

void
hash_table_update_impl(hash_table_t *table, hash_table_node_t *node, void *data)
{
  debug("hash_table_update_impl(table=0x%x, node=0x%x, data=0x%x)\n", table, node, data);
  
  node->data = data;
}

void
hash_table_insert_impl(hash_table_t *table, void const *key, void *data)
{
  hash_table_node_t *node;
  size_t            hash;
  
  debug("hash_table_insert_impl(table=0x%x, key=0x%x, data=0x%x)\n", table, key, data);
  
  hash = hash_table_bucket(table, key);
  node = table->nodes[hash];
  
  debug("hash_table_insert_impl: hash=%d ~> root-node=0x%x\n", hash, node);
  
  /* search within the bucket */
  while (node) {
    if (0 == table->comparator(node->key, key)) {
      /* found */
      debug("hash_table_insert_impl: hash=%d => node=0x%x\n", hash, node);
      node->data = data;
      return;
    }
    node = node->next;
  }
  
  /* create new data node */
  node               = MALLOC(hash_table_node_t);
  node->key          = table->duplicator(key);  
  node->data         = data;
  node->next         = table->nodes[hash];
  table->nodes[hash] = node;
  
  /* the table just got bigger */
  table->size++;
  
  debug("hash_table_insert_impl: hash=%d => node=0x%x)\n", hash, node);
}

bool
hash_table_insert(hash_table_t *table, void const *key, void *data)
{
  bool              existed;
  hash_table_node_t *node;
  
  debug("hash_table_insert(table=0x%x, key=0x%x, data=0x%x)\n", table, key, data);
  
  if (NULL == (node = hash_table_find(table, key))) {
    hash_table_insert_impl(table, key, data);
    existed = false;
  } else {
    hash_table_update_impl(table, node, data);
    existed = true;
  }
  
  debug("hash_table_insert: existed='%s'\n", bool_to_string(existed));
  
  return existed;
}

hash_table_node_t*
hash_table_find(hash_table_t *table, void const *key)
{
  hash_table_node_t *node;
  size_t       hash;
  
  debug("hash_table_find(table=0x%x, key=0x%x)\n", table, key);
  
  hash = hash_table_bucket(table, key);
  node = table->nodes[hash];
  
  debug("hash_table_find: hash=%d ~> root-node=0x%x\n", hash, node);
  
  while(node) {
    if (0 == table->comparator(node->key, key)) {
      /* found */
      debug("hash_table_find: hash=%d => node=0x%x, found!\n", hash, node);
      return node;
    }
    node = node->next;
  }
  
  return NULL;
}

#if 0
void
hash_table_resize(hash_table_t *table, size_t max_size)
{
  uint              i;
  hash_table_t      new_table;
  hash_table_node_s *node, *next;
  
  //table->nodes      = MALLOC_N(hash_table_node_t*, max_size);
  new_table.nodes      = MALLOC_N(hash_table_node_t*, max_size);
  new_table.max_size   = max_size;
  new_table.size       = table->size;
  new_table.hasher     = table->hasher;
  new_table.comparator = table->comparator;
  new_table.duplicator = table->duplicator;
  new_table.freer      = table->freer;
  
  for (i = 0; i < table->max_size; ++i) {
    node = table->nodes[i];
    for (node) {
      next = node->next;
      table_insert(&new_table, node->key, node->data);
      table_remove(table, node->key);
      node = next;
    }
  }
  
  safe_free(table->nodes);
  table->size  = new_table.size;
  table->nodes = new_table.nodes;

  return 0;
}
#endif

void
hash_table_debug(hash_table_t *table)
{
  uint              i;
  hash_table_node_t *node;
  
  if (verbose) {
    message("Hash Table Contents (%d/%d):\n", table->size, table->max_size);
    for (i = 0; i < table->max_size; ++i) {
      node = table->nodes[i];
      if (node) {
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
