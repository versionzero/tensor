
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

int 
storage_index_compare_for_compressed_row(const void *a, const void *b)
{
  int                result;
  coordinate_tuple_t *ta;
  coordinate_tuple_t *tb;
  
  ta = (coordinate_tuple_t*) a;
  tb = (coordinate_tuple_t*) b;
  
  if (0 == (result = ta->i - tb->i)) {
    if (0 == (result = ta->k - tb->k)) {
      result = ta->j - tb->j;
    }
  }
  
  return result;
}

int 
storage_index_compare_for_compressed_column(const void *a, const void *b)
{
  int                result;
  coordinate_tuple_t *ta;
  coordinate_tuple_t *tb;
  
  ta = (coordinate_tuple_t*) a;
  tb = (coordinate_tuple_t*) b;
  
  if (0 == (result = ta->j - tb->j)) {
    if (0 == (result = ta->k - tb->k)) {
      result = ta->i - tb->i;
    }
  }
  
  return result;
}

int 
storage_index_compare_for_compressed_tube(const void *a, const void *b)
{
  int                result;
  coordinate_tuple_t *ta;
  coordinate_tuple_t *tb;
  
  ta = (coordinate_tuple_t*) a;
  tb = (coordinate_tuple_t*) b;
    
  if (0 == (result = ta->k - tb->k)) {
    if (0 == (result = ta->j - tb->j)) {
      result = ta->i - tb->i;
    }
  }
  
  return result;
}

int 
storage_index_encode_for_compressed_row(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t *tuple;
  
  tuple      = (coordinate_tuple_t*) p;
  size       = 0;
  previous   = -1;
  
  debug("storage_index_encode_for_compressed_row(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  for (current = 0; current < nnz; ++current) {
    index = tuple[current].i;
    if (previous != index) {
      indices[size++] = current;
      previous        = index;
    }
  }
  indices[size++] = nnz;
  
  return size;
}

int 
storage_index_encode_for_compressed_column(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = -1;
  
  debug("storage_index_encode_for_compressed_column(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  for (current = 0; current < nnz; ++current) {
    index = tuple[current].j;
    if (previous != index) {
      indices[size++] = current;
      previous        = index;
    }
  }
  indices[size++] = nnz;
  
  return size;
}

int 
storage_index_encode_for_compressed_tube(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t*) p;
  size       = 0;
  previous   = -1;
  
  debug("storage_index_encode_for_compressed_tube(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  for (current = 0; current < nnz; ++current) {
    index = tuple[current].k;
    if (previous != index) {
      indices[size++] = current;
      previous        = index;
    }
  }
  indices[size++] = nnz;
  
  return size;
}

void
storage_index_copy_for_compressed_row(void *destination, void const *source, uint nnz)
{
  int i;
  storage_coordinate_t *s;
  storage_compressed_t const *d;
  
  s = (storage_coordinate_t*) source;
  d = (storage_compressed_t const*) destination;
  
  debug("storage_index_copy_for_compressed_row(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].j;
    d->KO[i] = s->tuples[i].k;
  }
}

void
storage_index_copy_for_compressed_column(void *destination, void const *source, uint nnz)
{
  int i;
  storage_coordinate_t *s;
  storage_compressed_t const *d;
  
  s = (storage_coordinate_t*) source;
  d = (storage_compressed_t const*) destination;
  
  debug("storage_index_copy_for_compressed_column(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].i;
    d->KO[i] = s->tuples[i].k;
  }
}

void
storage_index_copy_for_compressed_tube(void *destination, void const *source, uint nnz)
{
  int i;
  storage_coordinate_t *s;
  storage_compressed_t const *d;
  
  s = (storage_coordinate_t*) source;
  d = (storage_compressed_t const*) destination;
  
  debug("storage_index_copy_for_compressed_tube(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].i;
    d->KO[i] = s->tuples[i].j;
  }
}

void
storage_convert_from_coordinate_to_compressed_inplace(tensor_t *destination, tensor_t *source)
{
  int                  i, m, nnz;
  uint                 *indices;
  storage_base_t       *base;
  storage_compressed_t *d;
  storage_coordinate_t *s;
  coordinate_tuple_t   *tuples;
  double               *values;
  
  debug("storage_convert_from_coordinate_to_compressed_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  nnz    = source->nnz;
  values = source->values;
  s      = STORAGE_COORIDINATE(source);
  tuples = s->tuples;
  d      = STORAGE_COMPRESSED(destination);
  base   = (storage_base_t*) d;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  
  indices = MALLOC_N(uint, nnz);
  m       = (*base->callbacks->index_encode)(indices, tuples, nnz);
  
  debug("storage_convert_from_coordinate_to_compressed_inplace: compression=%d/%d\n", m, nnz);
  
  d->RO   = MALLOC_N(uint, m);
  d->size = m;
  
  for (i = 0; i < m; ++i) {
    d->RO[i] = indices[i];
  }
  
  safe_free(indices);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

storage_compressed_t*
storage_malloc_compressed(tensor_t const *tensor, bool naive)
{
  storage_base_t         *base;
  storage_compressed_t   *storage;
  conversion_callbacks_t *callbacks;
  
  debug("storage_malloc_compressed(tensor=0x%x, naive='%s')\n", 
	tensor, bool_to_string(naive));
  
  storage       = MALLOC(storage_compressed_t);
  storage->CO   = MALLOC_N(uint, tensor->nnz);
  storage->KO   = MALLOC_N(uint, tensor->nnz);
  storage->size = 0;
  storage->RO   = NULL;
  
  if (naive) {
    storage->size = tensor->nnz;
    storage->RO   = MALLOC_N(uint, tensor->nnz);
  }
  
  callbacks = MALLOC(conversion_callbacks_t);
  
  switch (tensor->orientation) {
  case orientation::row:
    callbacks->index_compare = &storage_index_compare_for_compressed_row;
    callbacks->index_encode  = &storage_index_encode_for_compressed_row;
    callbacks->index_copy    = &storage_index_copy_for_compressed_row;
    break;
  case orientation::column:
    callbacks->index_compare = &storage_index_compare_for_compressed_column;
    callbacks->index_encode  = &storage_index_encode_for_compressed_column;
    callbacks->index_copy    = &storage_index_copy_for_compressed_column;
    break;
  case orientation::tube:
    callbacks->index_compare = &storage_index_compare_for_compressed_tube;
    callbacks->index_encode  = &storage_index_encode_for_compressed_tube;
    callbacks->index_copy    = &storage_index_copy_for_compressed_tube;
    break;
  }
  
  base            = (storage_base_t*) storage;
  base->callbacks = callbacks;
  
  debug("storage_malloc_compressed: callbacks=0x%x\n", callbacks);
  debug("storage_malloc_compressed: storage->CO=0x%x\n", storage->CO);
  debug("storage_malloc_compressed: storage->KO=0x%x\n", storage->KO);
  debug("storage_malloc_compressed: storage->size (of RO)=%d\n", storage->size);
  debug("storage_malloc_compressed: storage->RO=0x%x\n", storage->RO);
  debug("storage_malloc_compressed: storage=0x%x\n", storage);
  
  return storage;
}
