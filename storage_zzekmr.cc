
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_r;

int 
storage_index_compare_for_zzekmr_row(void const *a, void const *b)
{
  uint                     ja, jb;
  int                      result;
  coordinate_tuple_t const *ta, *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ja = ta->j * g_r + ta->k;
  jb = tb->j * g_r + tb->k;
  
  /* We are doing just about exacly what the EKMR encoding does,
     except we reverse the order of every other row (we assume, for
     now, that all rows are non-empty).  We do this for a very simple,
     but elegant reason.  Take for instance the dense vector, sparse
     tensor product: Say we are at the end of a row, in terms of doing
     an operation.  The cache will have been primed with the elemets
     of one of the extremes of the vector.  If we naively pull in the
     next row, we must also pull in the matching vector elements.
     However, if we bring in the next row *in reverse order*, we will
     likely already have the matching vector elements in the
     cache. Thus, in general, we may not nessearily invalidate the
     existing cache lines. */
  
  if (0 == (result = ta->i - tb->i)) {
    if (ta->i % 2) { /* odd */
      result = jb - ja;
    } else {         /* even */
      result = ja - jb;
    }
  }
  
  return result;
}

void 
storage_index_encode_for_zzekmr_row(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("storage_index_encode_for_zzekmr_row(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
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
storage_index_copy_for_zzekmr_row(void *destination, void const *source, uint nnz)
{
  uint i;
  storage_coordinate_t const *s;
  storage_extended_t         *d;
  
  s = (storage_coordinate_t const*) source;
  d = (storage_extended_t*) destination;
  
  debug("storage_index_copy_for_zzekmr_row(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j * g_r + s->tuples[i].k;
  }
}

void
storage_convert_from_coordinate_to_zzekmr_inplace(tensor_t *destination, tensor_t *source)
{
  int                  i, nnz;
  storage_base_t       *base;
  storage_extended_t   *d;
  storage_coordinate_t *s;
  coordinate_tuple_t   *tuples;
  double               *values;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_EXTENDED(destination);
  
  debug("storage_convert_from_coordinate_to_zzekmr_inplace(destination=0x%x, source=0x%x)\n", d, s);
  
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

storage_extended_t*
storage_malloc_zzekmr(tensor_t const *tensor)
{
  storage_base_t         *base;
  storage_extended_t     *storage;
  conversion_callbacks_t *callbacks;
  
  debug("storage_malloc_zzekmr(tensor=0x%x)\n", tensor);
  
  storage                  = MALLOC(storage_extended_t);
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
    callbacks->index_compare = &storage_index_compare_for_zzekmr_row;
    callbacks->index_encode  = &storage_index_encode_for_zzekmr_row;
    callbacks->index_copy    = &storage_index_copy_for_zzekmr_row;
    break;
  default:
    die("Tensor orientation '%s' not yet supported.\n", orientation_to_string(tensor->orientation));
    break;
#if 0
  case orientation::column:
    storage->r               = tensor->m-1;
    storage->size            = tensor->n*tensor->l;
    callbacks->index_compare = &storage_index_compare_for_zzekmr_column;
    callbacks->index_encode  = &storage_index_encode_for_zzekmr_column;
    callbacks->index_copy    = &storage_index_copy_for_zzekmr_column;
    break;
  case orientation::tube:
    storage->r               = ?;
    callbacks->index_compare = &storage_index_compare_for_zzekmr_tube;
    callbacks->index_encode  = &storage_index_encode_for_zzekmr_tube;
    callbacks->index_copy    = &storage_index_copy_for_zzekmr_tube;
    break;
#endif
  }
  
  storage->size  += 1;
  storage->RO     = MALLOC_N(uint, storage->size);
  base            = (storage_base_t*) storage;
  base->callbacks = callbacks;
  
  debug("storage_malloc_zzekmr: callbacks=0x%x\n", callbacks);  
  debug("storage_malloc_zzekmr: storage->CK=0x%x\n", storage->CK);
  debug("storage_malloc_zzekmr: storage->size (of R)=%d\n", storage->size);
  debug("storage_malloc_zzekmr: storage->RO=0x%x\n", storage->RO);
  debug("storage_malloc_zzekmr: storage=0x%x\n", storage);
  
  return storage;
}