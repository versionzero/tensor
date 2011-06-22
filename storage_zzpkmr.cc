
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_r;

int 
storage_index_compare_for_zzpkmr_row(void const *a, void const *b)
{
  uint                     jka, jkb;
  int                      result;
  coordinate_tuple_t const *ta;
  coordinate_tuple_t const *tb;
  
  ta  = (coordinate_tuple_t const*) a;
  tb  = (coordinate_tuple_t const*) b;  
  jka = ta->j * g_r + ta->k;
  jkb = tb->j * g_r + tb->k;
  
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
      result = jkb - jka;
    } else {         /* even */
      result = jka - jkb;
    }
  }
  
  return result;
}

int 
storage_index_compare_for_zzpkmr_column(void const *a, void const *b)
{
  uint                     ika, ikb;
  int                      result;
  coordinate_tuple_t const *ta;
  coordinate_tuple_t const *tb;
  
  ta  = (coordinate_tuple_t const*) a;
  tb  = (coordinate_tuple_t const*) b;
  ika = ta->i * g_r + ta->k;
  ikb = tb->i * g_r + tb->k;
  
  if (0 == (result = ta->j - tb->j)) {
    if (ta->j % 2) { /* odd */
      result = ikb - ika;
    } else {         /* even */
      result = ika - ikb;
    }
  }
  
  return result;
}

int 
storage_index_compare_for_zzpkmr_tube(void const *a, void const *b)
{
  uint                     ija, ijb;
  int                      result;
  coordinate_tuple_t const *ta;
  coordinate_tuple_t const *tb;
  
  ta = (coordinate_tuple_t const*) a;
  tb = (coordinate_tuple_t const*) b;
  ija = ta->i * g_r + ta->j;
  ijb = tb->i * g_r + tb->j;
  
  if (0 == (result = ta->k - tb->k)) {
    result = ija - ijb;
  }
  
  return result;
}

void 
storage_index_encode_for_zzpkmr_row(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("storage_index_encode_for_zzpkmr_row(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
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
storage_index_encode_for_zzpkmr_column(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("storage_index_encode_for_zzpkmr_column(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
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
storage_index_encode_for_zzpkmr_tube(uint *indices, void const *p, uint nnz)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple;
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("storage_index_encode_for_zzpkmr_column(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
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
storage_index_copy_for_zzpkmr_row(void *destination, void const *source, uint nnz)
{
  uint i;
  storage_coordinate_t const *s;
  storage_extended_t         *d;
  
  s = (storage_coordinate_t const*) source;
  d = (storage_extended_t*) destination;
  
  debug("storage_index_copy_for_zzpkmr_row(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j * g_r + s->tuples[i].k;
  }
}

void
storage_index_copy_for_zzpkmr_column(void *destination, void const *source, uint nnz)
{
  uint i;
  storage_coordinate_t const *s;
  storage_extended_t         *d;
  
  s = (storage_coordinate_t const*) source;
  d = (storage_extended_t*) destination;
  
  debug("storage_index_copy_for_zzpkmr_column(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].i * g_r + s->tuples[i].k;
  }
}

void
storage_index_copy_for_zzpkmr_tube(void *destination, void const *source, uint nnz)
{
  uint i;
  storage_coordinate_t const *s;
  storage_extended_t         *d;
  
  s = (storage_coordinate_t const*) source;
  d = (storage_extended_t*) destination;
  
  debug("storage_index_copy_for_zzpkmr_tube(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].i * g_r + s->tuples[i].j;
  }
}

void
storage_convert_from_coordinate_to_zzpkmr_inplace(tensor_t *destination, tensor_t *source)
{
  int                  i, nnz;
  storage_base_t       *base;
  storage_extended_t   *d;
  storage_coordinate_t *s;
  coordinate_tuple_t   *tuples;
  double               *values;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_EXTENDED(destination);
  
  debug("storage_convert_from_coordinate_to_zzpkmr_inplace(destination=0x%x, source=0x%x)\n", d, s);
  
  base          = STORAGE_BASE(destination);
  nnz           = source->nnz;
  values        = source->values;
  g_r           = d->r;
  tuples        = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  (*base->callbacks->index_encode)(d->RO, tuples, nnz);
  (*base->callbacks->index_copy)(d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = values[tuples[i].index];
  }
}

storage_extended_t*
storage_malloc_zzpkmr(tensor_t const *tensor)
{
  storage_base_t         *base;
  storage_extended_t     *storage;
  conversion_callbacks_t *callbacks;
  
  debug("storage_malloc_zzpkmr(tensor=0x%x)\n", tensor);
  
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
    storage->r               = tensor->m-1;
    storage->size            = tensor->m;
    callbacks->index_compare = &storage_index_compare_for_zzpkmr_row;
    callbacks->index_encode  = &storage_index_encode_for_zzpkmr_row;
    callbacks->index_copy    = &storage_index_copy_for_zzpkmr_row;
    break;
  case orientation::column:
    storage->r               = tensor->m-1;
    storage->size            = tensor->n;
    callbacks->index_compare = &storage_index_compare_for_zzpkmr_column;
    callbacks->index_encode  = &storage_index_encode_for_zzpkmr_column;
    callbacks->index_copy    = &storage_index_copy_for_zzpkmr_column;
    break;
  case orientation::tube:
    storage->r               = tensor->n-1;
    storage->size            = tensor->l;
    callbacks->index_compare = &storage_index_compare_for_zzpkmr_tube;
    callbacks->index_encode  = &storage_index_encode_for_zzpkmr_tube;
    callbacks->index_copy    = &storage_index_copy_for_zzpkmr_tube;
    break;
  default:
    die("Tensor orientation '%s' not yet supported.\n", orientation_to_string(tensor->orientation));
    break;
  }
  
  storage->size  += 1;
  storage->RO     = MALLOC_N(uint, storage->size);
  base            = (storage_base_t*) storage;
  base->callbacks = callbacks;
  
  debug("storage_malloc_zzpkmr: callbacks=0x%x\n", callbacks);  
  debug("storage_malloc_zzpkmr: storage->CK=0x%x\n", storage->CK);
  debug("storage_malloc_zzpkmr: storage->size (of R)=%d\n", storage->size);
  debug("storage_malloc_zzpkmr: storage->RO=0x%x\n", storage->RO);
  debug("storage_malloc_zzpkmr: storage=0x%x\n", storage);
  
  return storage;
}
