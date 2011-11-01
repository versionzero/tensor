
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
copier_for_row(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->CO[i] = source->tuples[i].j;
  destination->KO[i] = source->tuples[i].k;
}

void
copier_for_column(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->CO[i] = source->tuples[i].i;
  destination->KO[i] = source->tuples[i].k;
}

void
copier_for_tube(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->CO[i] = source->tuples[i].k;
  destination->KO[i] = source->tuples[i].j;
}

void
tensor_storage_convert_from_coordinate_to_compressed(tensor_t *destination, tensor_t *source)
{
  int                         n, nnz;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_COMPRESSED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_compressed(destination=0x%x, source=0x%x)\n", destination, source);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  n      = source->n;
  values = source->values;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  d->rn = tensor_storage_index_encode(d->RO, n, tuples, nnz, base->callbacks->index_r_encoder);
  tensor_storage_copy(d, s, nnz, base->callbacks->index_copy);
  tensor_storage_copy(destination, source, nnz, (index_copy_t) &copier_for_values);
}

tensor_storage_compressed_t*
tensor_storage_malloc_compressed(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *storage;
  conversion_callbacks_t      *callbacks;
  
  superfluous("tensor_storage_malloc_compressed(tensor=0x%x)\n", tensor);
  
  storage     = MALLOC(tensor_storage_compressed_t);
  storage->rn = 0;
  storage->cn = tensor->nnz;
  storage->kn = tensor->nnz;
  storage->RO = NULL;
  storage->CO = MALLOC_N(uint, storage->cn);
  storage->TO = MALLOC_N(uint, storage->cn);
  storage->KO = MALLOC_N(uint, storage->kn);
  
  debug("tensor_storage_malloc_compressed: rn=%d, kn=%d\n", storage->rn, storage->kn);
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::row:
    storage->rn                = tensor->m;
    callbacks->index_compare   = (index_compare_t) &index_compare_ikj;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_copy      = (index_copy_t) &copier_for_row;
    break;
  case orientation::column:
    storage->rn                = tensor->n;
    callbacks->index_compare   = (index_compare_t) &index_compare_jki;
    callbacks->index_r_encoder = &encoder_for_j;
    callbacks->index_copy      = (index_copy_t) &copier_for_column;
    break;
  case orientation::tube:
    storage->rn                = tensor->l;
    callbacks->index_compare   = (index_compare_t) &index_compare_ijk;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_copy      = (index_copy_t) &copier_for_tube;
    break;
  default:
    die("tensor_storage_malloc_compressed: "
	"unknown or unsupported orientation %d.\n", 
	tensor->orientation);
    break;
  }
  
  storage->rn    += 1;
  storage->RO     = MALLOC_N(uint, storage->rn);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_compressed: callbacks=0x%x\n", callbacks);
  superfluous("tensor_storage_malloc_compressed: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_compressed: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_compressed: storage->size (of RO)=%d\n", storage->rn);
  superfluous("tensor_storage_malloc_compressed: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_compressed: storage=0x%x\n", storage);
  
  return storage;
}
