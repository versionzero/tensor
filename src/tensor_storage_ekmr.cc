
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
index_copy_for_ekmr_row(void *destination, void const *source, uint nnz)
{
  uint                              i, n;
  tensor_storage_coordinate_t const *s;
  tensor_storage_extended_t         *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_extended_t*) destination;
  n = d->rn - 1;
  
  debug("index_copy_for_ekmr_row(destination=0x%x, source=0x%x, nnz=%d)\n", d, s, nnz);
  
  for (i = 0; i < nnz; ++i) {
    d->CK[i] = s->tuples[i].j * n + s->tuples[i].k;
  }
}

void
tensor_storage_convert_from_coordinate_to_ekmr_inplace(tensor_t *destination, tensor_t *source)
{
  int                         nnz;
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
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  d->rn = tensor_storage_index_encode(d->RO, tuples, nnz, base->callbacks->index_r_encoder);
  tensor_storage_copy(d, s, nnz, base->callbacks->index_copy);
  tensor_storage_copy(destination, source, nnz, (index_copy_t) &copier_for_values);
}

tensor_storage_extended_t*
tensor_storage_malloc_ekmr(tensor_t const *tensor)
{
  tensor_storage_base_t     *base;
  tensor_storage_extended_t *storage;
  conversion_callbacks_t    *callbacks;
  
  superfluous("tensor_storage_malloc_ekmr(tensor=0x%x)\n", tensor);
  
  storage      = MALLOC(tensor_storage_extended_t);
  storage->rn  = 0;
  storage->ckn = tensor->nnz;
  storage->RO  = NULL;
  storage->CK  = MALLOC_N(uint, storage->ckn);
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::row:
    storage->rn                = tensor->n;
    callbacks->index_compare   = (index_compare_t) &index_compare_ijk;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_copy      = &index_copy_for_ekmr_row;
    break;
  default:
    die("Tensor orientation '%s' not yet supported.\n", orientation_to_string(tensor->orientation));
    break;
  }
  
  storage->rn    += 1;
  storage->RO     = MALLOC_N(uint, storage->rn);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_ekmr: callbacks=0x%x\n", callbacks);  
  superfluous("tensor_storage_malloc_ekmr: storage->CK=0x%x\n", storage->CK);
  superfluous("tensor_storage_malloc_ekmr: storage->size (of R)=%d\n", storage->rn);
  superfluous("tensor_storage_malloc_ekmr: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_ekmr: storage=0x%x\n", storage);
  
  return storage;
}
