
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_r;

int 
tensor_storage_index_compare_for_compressed_slice_lateral(void const *a, void const *b)
{
  uint                     ka, kb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ka = ta->k * g_r + ta->j;
  kb = tb->k * g_r + tb->j;
  
  if (0 == (result = ka - kb)) {
    result = ta->i - tb->i;
  }
  
  return result;
}

int 
tensor_storage_index_compare_for_compressed_slice_horizontal(void const *a, void const *b)
{
  uint                     ja, jb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ja = ta->j * g_r + ta->k;
  jb = tb->j * g_r + tb->k;
  
  if (0 == (result = ta->i - tb->i)) {
    result = ja - jb;
  }
  
  return result;
}

int
tensor_storage_index_compare_for_compressed_slice_frontal(void const *a, void const *b)
{
  uint                     ja, jb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ja = ta->j * g_r + ta->k;
  jb = tb->j * g_r + tb->k;
  
  if (0 == (result = ja - jb)) {
    result = ta->i - tb->i;
  }
  
  return result;
}

void 
tensor_storage_index_encode_for_compressed_slice_lateral(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_compressed_slice_lateral(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
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
tensor_storage_index_encode_for_compressed_slice_horizontal(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_compressed_slice_horizontal(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
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
tensor_storage_index_encode_for_compressed_slice_frontal(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_compressed_slice_frontal(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
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
tensor_storage_index_copy_for_compressed_slice_lateral(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_lateral(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].k;
  }
}

void
tensor_storage_index_copy_for_compressed_slice_horizontal(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_horizontal(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].k;
  }
}

void
tensor_storage_index_copy_for_compressed_slice_frontal(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_frontal(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j;
  }
}

void
tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(tensor_t *destination, tensor_t *source)
{
  int                         i, nnz;
  tensor_storage_base_t       *base;
  tensor_storage_extended_t   *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_EXTENDED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  values = source->values;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  (*base->callbacks->index_encode)(d->RO, tuples, nnz);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

tensor_storage_extended_t*
tensor_storage_malloc_compressed_slice(tensor_t const *tensor)
{
  tensor_storage_base_t     *base;
  tensor_storage_extended_t *storage;
  conversion_callbacks_t    *callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice(tensor=0x%x)\n", tensor);
  
  storage       = MALLOC(tensor_storage_extended_t);
  storage->CK   = MALLOC_N(uint, tensor->nnz);
  storage->size = 0;
  storage->RO   = NULL;
  
  callbacks     = MALLOC(conversion_callbacks_t);
  
  switch (tensor->orientation) {
  case orientation::lateral:
    /* rows * tubes */
    storage->size            = tensor->m * tensor->l;
    storage->r               = tensor->m;
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_lateral;
    callbacks->index_encode  = &tensor_storage_index_encode_for_compressed_slice_lateral;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_lateral;
    break;
  case orientation::horizontal:
    /* columns * tubes */
    storage->size            = tensor->n * tensor->l;
    storage->r               = tensor->n;
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_horizontal;
    callbacks->index_encode  = &tensor_storage_index_encode_for_compressed_slice_horizontal;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_horizontal;
    break;
  case orientation::frontal:
    /* rows * columns */
    storage->size            = tensor->m * tensor->n;
    storage->r               = tensor->m;
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_frontal;
    callbacks->index_encode  = &tensor_storage_index_encode_for_compressed_slice_frontal;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_frontal;
    break;
  default:
    die("Unknown or unsupported orientation %d.\n", tensor->orientation);
    break;
  }
  
  storage->size  += 1;
  storage->RO     = MALLOC_N(uint, storage->size);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice: callbacks=0x%x\n", callbacks);
  superfluous("tensor_storage_malloc_compressed_slice: storage->CK=0x%x\n", storage->CK);
  superfluous("tensor_storage_malloc_compressed_slice: storage->size (of RO)=%d\n", storage->size);
  superfluous("tensor_storage_malloc_compressed_slice: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_compressed_slice: storage=0x%x\n", storage);
  
  return storage;
}
