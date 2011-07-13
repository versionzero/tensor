
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_r;

int 
tensor_storage_index_compare_for_ekmr_row(void const *a, void const *b)
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

void 
tensor_storage_index_encode_for_ekmr_row(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode_for_ekmr_row(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
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
tensor_storage_index_copy_for_ekmr_row(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t         *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  
  debug("tensor_storage_index_copy_for_ekmr_row(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j * g_r + s->tuples[i].k;
  }
}

void
tensor_storage_convert_from_coordinate_to_ekmr_inplace(tensor_t *destination, tensor_t *source)
{
  int                         i, nnz;
  tensor_storage_base_t       *base;
  tensor_storage_extended_t   *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_EXTENDED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_ekmr_inplace(destination=0x%x, source=0x%x)\n", d, s);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  values = source->values;
  g_r    = d->r;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  (*base->callbacks->index_encode)(d->RO, tuples, nnz);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

tensor_storage_extended_t*
tensor_storage_malloc_ekmr(tensor_t const *tensor)
{
  tensor_storage_base_t     *base;
  tensor_storage_extended_t *storage;
  conversion_callbacks_t    *callbacks;
  
  superfluous("tensor_storage_malloc_ekmr(tensor=0x%x)\n", tensor);
  
  storage                  = MALLOC(tensor_storage_extended_t);
  storage->CK              = MALLOC_N(uint, tensor->nnz);
  storage->RO              = NULL;
  storage->size            = 0;
  
  callbacks                = MALLOC(conversion_callbacks_t);
  callbacks->index_compare = NULL;
  callbacks->index_encode  = NULL;
  callbacks->index_copy	   = NULL;
  
  switch (tensor->orientation) {
  case orientation::row:
    storage->r               = tensor->n;
    storage->size            = tensor->m;
    callbacks->index_compare = &tensor_storage_index_compare_for_ekmr_row;
    callbacks->index_encode  = &tensor_storage_index_encode_for_ekmr_row;
    callbacks->index_copy    = &tensor_storage_index_copy_for_ekmr_row;
    break;
  default:
    die("Tensor orientation '%s' not yet supported.\n", orientation_to_string(tensor->orientation));
    break;
#if 0
  case orientation::column:
    storage->r               = tensor->m-1;
    storage->size            = tensor->n*tensor->l;
    callbacks->index_compare = &tensor_storage_index_compare_for_ekmr_column;
    callbacks->index_encode  = &tensor_storage_index_encode_for_ekmr_column;
    callbacks->index_copy    = &tensor_storage_index_copy_for_ekmr_column;
    break;
  case orientation::tube:
    storage->r               = ?;
    callbacks->index_compare = &tensor_storage_index_compare_for_ekmr_tube;
    callbacks->index_encode  = &tensor_storage_index_encode_for_ekmr_tube;
    callbacks->index_copy    = &tensor_storage_index_copy_for_ekmr_tube;
    break;
#endif
  }
  
  storage->size  += 1;
  storage->RO     = MALLOC_N(uint, storage->size);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_ekmr: callbacks=0x%x\n", callbacks);  
  superfluous("tensor_storage_malloc_ekmr: storage->CK=0x%x\n", storage->CK);
  superfluous("tensor_storage_malloc_ekmr: storage->size (of R)=%d\n", storage->size);
  superfluous("tensor_storage_malloc_ekmr: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_ekmr: storage=0x%x\n", storage);
  
  return storage;
}
