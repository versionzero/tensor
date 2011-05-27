
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_r;

int 
storage_index_compare_for_ekmr_row(const void *a, const void *b)
{
  uint               ja, jb;
  int                result;
  coordinate_tuple_t *ta;
  coordinate_tuple_t *tb;
  
  ta = (coordinate_tuple_t*) a;
  tb = (coordinate_tuple_t*) b;
  ja = ta->j * g_r + ta->k;
  jb = tb->j * g_r + tb->k;
  
  if (0 == (result = ta->i - tb->i)) {
    result = ja - jb;
  }
  
  return result;
}

int 
storage_index_encode_for_ekmr_row(uint *indices, void const *p, uint nnz)
{
  int previous;
  uint size, index, current;
  coordinate_tuple_t *tuple;
  
  tuple      = (coordinate_tuple_t*) p;
  size       = 0;
  previous   = -1;
  
  debug("storage_index_encode_for_ekmr_row(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
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

void
storage_index_copy_for_ekmr_row(void *destination, void const *source, uint nnz)
{
  int i, j;
  storage_coordinate_t *s;
  storage_ekmr_t const *d;
  
  s = (storage_coordinate_t*) source;
  d = (storage_ekmr_t const*) destination;
  
  debug("storage_index_copy_for_ekmr_row(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j * g_r + s->tuples[i].k;
  }
}

void
storage_convert_from_coordinate_to_ekmr_inplace(tensor_t *destination, tensor_t *source)
{
  int                  i, m, nnz;
  uint                 *indices;
  storage_base_t       *base;
  storage_ekmr_t       *d;
  storage_coordinate_t *s;
  coordinate_tuple_t   *tuples;
  double               *values;
  
  debug("storage_convert_from_coordinate_to_ekmr_inplace(destination=0x%x, source=0x%x)\n", d, s);
  
  s       = STORAGE_COORIDINATE(source);
  d       = STORAGE_EKMR(destination);
  base    = STORAGE_BASE(destination);
  nnz     = source->nnz;
  values  = source->values;
  g_r     = d->r;
  tuples  = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  
  indices = MALLOC_N(uint, nnz);
  m       = (*base->callbacks->index_encode)(indices, tuples, nnz);
  
  debug("storage_convert_from_coordinate_to_ekmr_inplace: compression=%d/%d\n", m, nnz);
  
  d->R    = MALLOC_N(uint, m);
  d->size = m;
  
  for (i = 0; i < m; ++i) {
    d->R[i] = indices[i];
  }
  
  safe_free(indices);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

storage_ekmr_t*
storage_malloc_ekmr(tensor_t const *tensor, bool naive)
{
  storage_base_t         *base;
  storage_ekmr_t         *storage;
  conversion_callbacks_t *callbacks;
  
  debug("storage_malloc_ekmr(tensor=0x%x, naive='%s')\n", 
	tensor, bool_to_string(naive));
  
  storage       = MALLOC(storage_ekmr_t);
  storage->CK   = MALLOC_N(uint, tensor->nnz);
  storage->size = 0;
  storage->R    = NULL;
  
  if (naive) {
    storage->size = tensor->nnz;
    storage->R    = MALLOC_N(uint, tensor->nnz);
  }
  
  callbacks                = MALLOC(conversion_callbacks_t);
  callbacks->index_compare = NULL;
  callbacks->index_encode  = NULL;
  callbacks->index_copy	   = NULL;
  
  switch (tensor->orientation) {
  case orientation::row:
    storage->r               = tensor->m;
    callbacks->index_compare = &storage_index_compare_for_ekmr_row;
    callbacks->index_encode  = &storage_index_encode_for_ekmr_row;
    callbacks->index_copy    = &storage_index_copy_for_ekmr_row;
    break;
  default:
    die("Tensor orientation '%s' not yet supported.\n", orientation_to_string(tensor->orientation));
    break;
#if 0
  case orientation::column:
    storage->r               = ?;
    callbacks->index_compare = &storage_index_compare_for_ekmr_column;
    callbacks->index_encode  = &storage_index_encode_for_ekmr_column;
    callbacks->index_copy    = &storage_index_copy_for_ekmr_column;
    break;
  case orientation::tube:
    storage->r               = ?;
    callbacks->index_compare = &storage_index_compare_for_ekmr_tube;
    callbacks->index_encode  = &storage_index_encode_for_ekmr_tube;
    callbacks->index_copy    = &storage_index_copy_for_ekmr_tube;
    break;
#endif
  }
  
  base            = (storage_base_t*) storage;
  base->callbacks = callbacks;
  
  debug("storage_malloc_ekmr: callbacks=0x%x\n", callbacks);  
  debug("storage_malloc_ekmr: storage->CK=0x%x\n", storage->CK);
  debug("storage_malloc_ekmr: storage->size (of R)=%d\n", storage->size);
  debug("storage_malloc_ekmr: storage->R=0x%x\n", storage->R);
  debug("storage_malloc_ekmr: storage=0x%x\n", storage);
  
  return storage;
}
