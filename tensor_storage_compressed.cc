
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

int 
tensor_storage_index_compare_for_compressed_row(void const *a, void const *b)
{
  int result;
  coordinate_tuple_t const *ta;
  coordinate_tuple_t const *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  
  if (0 == (result = ta->i - tb->i)) {
    if (0 == (result = ta->k - tb->k)) {
      result = ta->j - tb->j;
    }
  }
  
  return result;
}

int 
tensor_storage_index_compare_for_compressed_column(void const *a, void const *b)
{
  int result;
  coordinate_tuple_t const *ta;
  coordinate_tuple_t const *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  
  if (0 == (result = ta->j - tb->j)) {
    if (0 == (result = ta->k - tb->k)) {
      result = ta->i - tb->i;
    }
  }
  
  return result;
}

int 
tensor_storage_index_compare_for_compressed_tube(void const *a, void const *b)
{
  int result;
  coordinate_tuple_t const *ta;
  coordinate_tuple_t const *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
    
  if (0 == (result = ta->k - tb->k)) {
    if (0 == (result = ta->j - tb->j)) {
      result = ta->i - tb->i;
    }
  }
  
  return result;
}

void 
tensor_storage_index_encode_for_compressed_row(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_compressed_row(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  indices[size++] = 0;
  for (current = 0; current < nnz; ++current) {
    index = tuple[current].i;
    if (previous != index) {
      indices[size++] = current;
      previous        = index;
    }
  }
  indices[size++] = nnz;
}

void 
tensor_storage_index_encode_for_compressed_column(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_compressed_column(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  indices[size++] = 0;
  for (current = 0; current < nnz; ++current) {
    index = tuple[current].j;
    if (previous != index) {
      indices[size++] = current;
      previous        = index;
    }
  }
  indices[size++] = nnz;
}

void 
tensor_storage_index_encode_for_compressed_tube(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_compressed_tube(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  indices[size++] = 0;
  for (current = 0; current < nnz; ++current) {
    index = tuple[current].k;
    if (previous != index) {
      indices[size++] = current;
      previous        = index;
    }
  }
  indices[size++] = nnz;
}

void
tensor_storage_index_copy_for_compressed_row(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_row(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].j;
    d->KO[i] = s->tuples[i].k;
  }
}

void
tensor_storage_index_copy_for_compressed_column(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_column(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].i;
    d->KO[i] = s->tuples[i].k;
  }
}

void
tensor_storage_index_copy_for_compressed_tube(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_tube(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].i;
    d->KO[i] = s->tuples[i].j;
  }
}

void
tensor_storage_convert_from_coordinate_to_compressed_inplace(tensor_t *destination, tensor_t *source)
{
  int                  i, nnz;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t   *tuples;
  double               *values;
  
  debug("tensor_storage_convert_from_coordinate_to_compressed_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  nnz    = source->nnz;
  values = source->values;
  s      = STORAGE_COORIDINATE(source);
  tuples = s->tuples;
  d      = STORAGE_COMPRESSED(destination);
  base   = (tensor_storage_base_t*) d;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  (*base->callbacks->index_encode)(d->RO, tuples, nnz);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

tensor_storage_compressed_t*
tensor_storage_malloc_compressed(tensor_t const *tensor)
{
  tensor_storage_base_t         *base;
  tensor_storage_compressed_t   *storage;
  conversion_callbacks_t *callbacks;
  
  superfluous("tensor_storage_malloc_compressed(tensor=0x%x)\n", tensor);
  
  storage       = MALLOC(tensor_storage_compressed_t);
  storage->CO   = MALLOC_N(uint, tensor->nnz);
  storage->KO   = MALLOC_N(uint, tensor->nnz);
  storage->size = 0;
  storage->RO   = NULL;
  
  callbacks = MALLOC(conversion_callbacks_t);
  
  switch (tensor->orientation) {
  case orientation::row:
    storage->size            = tensor->m;
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_row;
    callbacks->index_encode  = &tensor_storage_index_encode_for_compressed_row;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_row;
    break;
  case orientation::column:
    storage->size            = tensor->n;
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_column;
    callbacks->index_encode  = &tensor_storage_index_encode_for_compressed_column;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_column;
    break;
  case orientation::tube:
    storage->size            = tensor->l;
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_tube;
    callbacks->index_encode  = &tensor_storage_index_encode_for_compressed_tube;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_tube;
    break;
  default:
    die("Unknown or unsupported orientation %d.\n", tensor->orientation);
    break;
  }
  
  storage->size  += 1;
  storage->RO     = MALLOC_N(uint, storage->size);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_compressed: callbacks=0x%x\n", callbacks);
  superfluous("tensor_storage_malloc_compressed: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_compressed: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_compressed: storage->size (of RO)=%d\n", storage->size);
  superfluous("tensor_storage_malloc_compressed: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_compressed: storage=0x%x\n", storage);
  
  return storage;
}
