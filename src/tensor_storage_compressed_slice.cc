
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
copier_for_slice_frontal(void *destination, void const *source, uint nnz)
{
  uint                              i, n;
  tensor_storage_coordinate_t const *s;
  tensor_storage_compressed_t       *d;
  
  s = (tensor_storage_coordinate_t const*) source;
  d = (tensor_storage_compressed_t*) destination;
  n = d->rn - 1;
  
  debug("copier_for_slice_frontal(destination=0x%x, source=0x%x, nnz=%d, n=%d)\n", d, s, nnz, n);
  
  for (i = 0; i < nnz; ++i) {
    d->CO[i] = s->tuples[i].i * n + s->tuples[i].j;
  }
}

void
tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(tensor_t *destination, tensor_t *source)
{
  uint                        nnz;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  
  debug("tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  values = source->values;
  tuples = s->tuples;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  d->rn = tensor_storage_index_encode(d->RO, tuples, nnz, base->callbacks->index_r_encoder);
  tensor_storage_copy(d, s, nnz, base->callbacks->index_copy);
  tensor_storage_copy(destination, source, nnz, (index_copy_t) &copier_for_values);  
}

tensor_storage_compressed_t*
tensor_storage_malloc_compressed_slice(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *storage;
  conversion_callbacks_t      *callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice(tensor=0x%x)\n", tensor);
  
  storage     = MALLOC(tensor_storage_compressed_t);
  storage->rn = tensor->n * tensor->n + 1;
  storage->kn = tensor->nnz;
  storage->RO = MALLOC_N(uint, storage->rn);
  storage->CO = MALLOC_N(uint, storage->cn);
  storage->KO = MALLOC_N(uint, storage->kn);
  
  debug("tensor_storage_malloc_compressed_slice: rn=%d, cn=%d, tn=%d, kn=%d\n",
	storage->rn, storage->cn, storage->kn);
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_c_encoder = NULL;
  callbacks->index_t_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::lateral:
    callbacks->index_compare   = (index_compare_t) &index_compare_jik;
    callbacks->index_r_encoder = &encoder_for_j;
    callbacks->index_c_encoder = &encoder_for_i;
    callbacks->index_t_encoder = &encoder_for_k;
    callbacks->index_copy      = (index_copy_t) &copier_for_k;
    break;
  case orientation::horizontal:
    callbacks->index_compare   = (index_compare_t) &index_compare_ijk;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_c_encoder = &encoder_for_j;
    callbacks->index_t_encoder = &encoder_for_k;
    callbacks->index_copy      = (index_copy_t) &copier_for_k;
    break;
  case orientation::frontal:
    callbacks->index_compare   = (index_compare_t) &index_compare_kij;
    callbacks->index_r_encoder = &encoder_for_k;
    callbacks->index_copy      = &copier_for_slice_frontal;
    break;
  default:
    die("tensor_storage_malloc_compressed_slice: "
	"unknown or unsupported orientation %d.\n", 
	tensor->orientation);
    break;
  }
  
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice: callbacks=0x%x\n", callbacks);
  superfluous("tensor_storage_malloc_compressed_slice: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_compressed_slice: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_compressed_slice: storage->CO=0x%x\n", storage->TO);
  superfluous("tensor_storage_malloc_compressed_slice: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_compressed_slice: storage=0x%x\n", storage);
  
  return storage;
}
