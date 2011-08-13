
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_n;

uint
encoder_for_lateral(coordinate_tuple_t const *tuple)
{
  return tuple->j * g_n + tuple->k;
}

uint
encoder_for_horizontal(coordinate_tuple_t const *tuple)
{
  return tuple->i * g_n + tuple->k;
}

uint
encoder_for_frontal(coordinate_tuple_t const *tuple)
{
  return tuple->k * g_n + tuple->i;
}

int
tensor_storage_index_compare_for_compressed_slice_lateral(void const *a, void const *b)
{
  uint                     ja, jb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ja = ta->j * g_n + ta->k;
  jb = tb->j * g_n + tb->k;
  
  if (0 == (result = ja - jb)) {
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
  ka = ta->i * g_n + ta->k;
  kb = tb->i * g_n + tb->k;
  
  if (0 == (result = ka - kb)) {
    result = ta->j - tb->j;
  }
  
  return result;
}

int 
tensor_storage_index_compare_for_compressed_slice_frontal(void const *a, void const *b)
{
  uint                     ka, kb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ka = ta->k * g_n + ta->i;
  kb = tb->k * g_n + tb->i;
  
  if (0 == (result = ka - kb)) {
    result = ta->j - tb->j;
  }
  
  return result;
}

void
tensor_storage_index_copy_for_compressed_slice_lateral(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t       *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_lateral(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->KO[i] = s->tuples[i].j;
  }
}

void
tensor_storage_index_copy_for_compressed_slice_horizontal(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t       *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_horizontal(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->KO[i] = s->tuples[i].j;
  }
}

void
tensor_storage_index_copy_for_compressed_slice_frontal(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t       *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  
  debug("tensor_storage_index_copy_for_compressed_slice_frontal(destination=0x%x, source=0x%x)\n", d, s);
  
  for (i = 0; i < nnz; ++i) {
    d->KO[i] = s->tuples[i].j;
  }
}

void
tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(tensor_t *destination, tensor_t *source)
{
  int                         i, nnz;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_COMPRESSED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  values = source->values;
  g_n    = source->n;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  d->rn = tensor_storage_index_encode(d->RO, tuples, nnz, base->callbacks->index_encoder);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

tensor_storage_compressed_t*
tensor_storage_malloc_compressed_slice(tensor_t const *tensor)
{
  uint                        n;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *storage;
  conversion_callbacks_t      *callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice(tensor=0x%x)\n", tensor);
  
  storage                  = MALLOC(tensor_storage_compressed_t);
  storage->rn              = tensor->n + 1;
  storage->cn              = n * tensor->n;
  storage->kn              = tensor->nnz;
  storage->RO              = MALLOC_N(uint, storage->rn);
  storage->CO              = MALLOC_N(uint, storage->cn);
  storage->KO              = MALLOC_N(uint, storage->kn);
  
  callbacks                = MALLOC(conversion_callbacks_t);
  callbacks->index_compare = NULL;
  callbacks->index_encoder = NULL;
  callbacks->index_copy	   = NULL;
  
  switch (tensor->orientation) {
  case orientation::lateral:
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_lateral;
    callbacks->index_encoder = &encoder_for_lateral;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_lateral;
    break;
  case orientation::horizontal:
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_horizontal;
    callbacks->index_encoder = &encoder_for_horizontal;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_horizontal;
    break;
  case orientation::frontal:
    callbacks->index_compare = &tensor_storage_index_compare_for_compressed_slice_frontal;
    callbacks->index_encoder = &encoder_for_frontal;
    callbacks->index_copy    = &tensor_storage_index_copy_for_compressed_slice_frontal;
    break;
  default:
    die("Unknown or unsupported orientation %d.\n", tensor->orientation);
    break;
  }
  
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice: callbacks=0x%x\n", callbacks);
  superfluous("tensor_storage_malloc_compressed: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_compressed: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_compressed_slice: storage->size (of RO)=%d\n", storage->rn);
  superfluous("tensor_storage_malloc_compressed_slice: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_compressed_slice: storage=0x%x\n", storage);
  
  return storage;
}
