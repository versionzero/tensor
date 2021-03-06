
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
tensor_storage_index_compare_for_zzekmr_row(void const *a, void const *b)
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
tensor_storage_index_copy_for_zzekmr_row(void *destination, void const *source, uint nnz)
{
  uint i;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t         *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  
  debug("tensor_storage_index_copy_for_zzekmr_row(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j * g_r + s->tuples[i].k;
  }
}

void
tensor_storage_convert_from_coordinate_to_zzekmr(tensor_t *destination, tensor_t *source)
{
  uint                  i, n, nnz;
  tensor_storage_base_t       *base;
  tensor_storage_extended_t   *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t   *tuples;
  double               *values;
   
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_EXTENDED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_zzekmr(destination=0x%x, source=0x%x)\n", d, s);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  n      = source->n;
  values = source->values;
  g_r    = d->rn;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  d->rn = tensor_storage_index_encode(d->RO, n, tuples, nnz, base->callbacks->index_r_encoder);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

tensor_storage_extended_t*
tensor_storage_malloc_zzekmr(tensor_t const *tensor)
{
  tensor_storage_base_t     *base;
  tensor_storage_extended_t *storage;
  conversion_callbacks_t    *callbacks;
  
  superfluous("tensor_storage_malloc_zzekmr(tensor=0x%x)\n", tensor);
  
  storage     = MALLOC(tensor_storage_extended_t);
  storage->CK = MALLOC_N(uint, tensor->nnz);
  storage->RO = NULL;
  storage->rn = 0;
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::row:
    storage->rn                = tensor->n;
    callbacks->index_compare   = &tensor_storage_index_compare_for_zzekmr_row;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_copy      = &tensor_storage_index_copy_for_zzekmr_row;
    break;
  default:
    die("Tensor orientation '%s' not yet supported.\n", orientation_to_string(tensor->orientation));
    break;
  }
  
  storage->rn    += 1;
  storage->RO     = MALLOC_N(uint, storage->rn);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_zzekmr: callbacks=0x%x\n", callbacks);  
  superfluous("tensor_storage_malloc_zzekmr: storage->CK=0x%x\n", storage->CK);
  superfluous("tensor_storage_malloc_zzekmr: storage->size (of R)=%d\n", storage->rn);
  superfluous("tensor_storage_malloc_zzekmr: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_zzekmr: storage=0x%x\n", storage);
  
  return storage;
}
