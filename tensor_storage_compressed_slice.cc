
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(tensor_t *destination, tensor_t *source)
{
  uint                        nnz;
  uint                        rn, cn, index, current, prev_ri, prev_ci;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  uint                        *R, *C, *K;
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_COMPRESSED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(destination=0x%x, source=0x%x)\n", destination, source);
  
  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  values = source->values;
  tuples = s->tuples;
  R      = d->RO;
  C      = d->CO;
  K      = d->KO;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
#if 0
  d->rn = tensor_storage_index_encode(d->RO, tuples, nnz, base->callbacks->index_r_encoder);
  d->cn = tensor_storage_index_encode(d->CO, tuples, nnz, base->callbacks->index_c_encoder);
#endif
  tensor_storage_copy(d, s, nnz, base->callbacks->index_copy);
  tensor_storage_copy(destination, source, nnz, (index_copy_t) &copier_for_values);
  
  
  rn = cn = 0;
  prev_ri = base->callbacks->index_r_encoder(&tuples[0]);
  prev_ci = base->callbacks->index_c_encoder(&tuples[0]);
  R[rn++] = 0;
  C[cn++] = 0;
  
  for (current = 0; current < nnz; ++current) {
    DEBUG("i=%u, j=%u, k=%u, index=%u\n", 
	  tuples[current].i, tuples[current].j, 
	  tuples[current].k, tuples[current].index);
    index = base->callbacks->index_c_encoder(&tuples[current]);
    if (prev_ci != index) {
      DEBUG("C[size=%u]=%u\n", cn, current);
      C[cn++] = current;
      prev_ci = index;
    }
    index = base->callbacks->index_r_encoder(&tuples[current]);
    if (prev_ri != index) {
      DEBUG("R[size=%u]=%u\n", rn, current);
      R[rn++] = cn-1;
      prev_ri = index;
    }
  }
  
  DEBUG("C[size=%u]=%u\n", cn, nnz);
  DEBUG("R[size=%u]=%u\n", rn, cn);
  
  C[cn++] = nnz;
  R[rn++] = cn-1;
  
  DEBUG("rn=%u\n", rn);
  DEBUG("cn=%u\n", cn);
  
  d->rn = rn;
  d->cn = cn;
}

tensor_storage_compressed_t*
tensor_storage_malloc_compressed_slice(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *storage;
  conversion_callbacks_t      *callbacks;
  
  superfluous("tensor_storage_malloc_compressed_slice(tensor=0x%x)\n", tensor);
  
  storage     = MALLOC(tensor_storage_compressed_t);
  storage->rn = tensor->n + 1;
  storage->cn = tensor->n * tensor->n + 1;
  storage->kn = tensor->nnz;
  storage->RO = MALLOC_N(uint, storage->rn);
  storage->CO = MALLOC_N(uint, storage->cn);
  storage->KO = MALLOC_N(uint, storage->kn);
  
  debug("tensor_storage_malloc_compressed_slice: rn=%d, cn=%d, kn=%d\n", 
	storage->rn, storage->cn, storage->kn);
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_c_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::lateral:
    callbacks->index_compare   = (index_compare_t) &index_compare_jik;
    callbacks->index_r_encoder = &encoder_for_j;
    callbacks->index_c_encoder = &encoder_for_i;
    callbacks->index_copy      = (index_copy_t) &copier_for_k;
    break;
  case orientation::horizontal:
    callbacks->index_compare   = (index_compare_t) &index_compare_ijk;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_c_encoder = &encoder_for_j;
    callbacks->index_copy      = (index_copy_t) &copier_for_k;
    break;
  case orientation::frontal:
    callbacks->index_compare   = (index_compare_t) &index_compare_kij;
    callbacks->index_r_encoder = &encoder_for_k;
    callbacks->index_c_encoder = &encoder_for_i;
    callbacks->index_copy      = (index_copy_t) &copier_for_j;
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
  superfluous("tensor_storage_malloc_compressed: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_compressed: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_compressed_slice: storage->size (of RO)=%d\n", storage->rn);
  superfluous("tensor_storage_malloc_compressed_slice: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_malloc_compressed_slice: storage=0x%x\n", storage);
  
  return storage;
}
