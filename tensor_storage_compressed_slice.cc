
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
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
  uint                     ka, kb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ka = ta->k * g_r + ta->i;
  kb = tb->k * g_r + tb->i;
  
  if (0 == (result = ka - kb)) {
    result = ta->j - tb->j;
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
tensor_storage_index_copy_for_compressed_slice_lateral(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_lateral(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].k * g_r + s->tuples[i].j;
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
    d->CK[i] = s->tuples[i].k * g_r + s->tuples[i].i;
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
    d->CK[i] = s->tuples[i].j * g_r + s->tuples[i].k;
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
  g_r    = d->r;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  d->size = tensor_storage_index_encode(d->RO, tuples, nnz, base->callbacks->index_encoder);
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
  
  storage                  = MALLOC(tensor_storage_extended_t);
  storage->CK              = MALLOC_N(uint, tensor->nnz);
  storage->size            = 0;
  storage->RO              = NULL;
  
  callbacks                = MALLOC(conversion_callbacks_t);
  callbacks->index_compare = NULL;
  callbacks->index_encoder = NULL;
  callbacks->index_copy	   = NULL;
  storage->size            = tensor->n * tensor->n;
  storage->r               = tensor->n;
  
  switch (tensor->orientation) {
  case orientation::lateral:
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_lateral;
    callbacks->index_encoder = &encoder_for_i;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_lateral;
    break;
  case orientation::horizontal:
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_horizontal;
    callbacks->index_encoder = &encoder_for_j;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_horizontal;
    break;
  case orientation::frontal:
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_frontal;
    callbacks->index_encoder = &encoder_for_j;
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
