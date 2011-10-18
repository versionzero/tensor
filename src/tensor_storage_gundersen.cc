
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
tensor_storage_convert_from_coordinate_to_gundersen(tensor_t *destination, tensor_t *source)
{
  uint                        nnz;
  uint                        rn, cn, index, current, prev_ri, prev_ci;
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *d;
  tensor_storage_coordinate_t *s;
  coordinate_tuple_t          *tuples;
  double                      *values;
  uint                        *R, *C, *K;
  index_encoder_t             r_encoder, c_encoder;
  
  debug("tensor_storage_convert_from_coordinate_to_gundersen(destination=0x%x, source=0x%x)\n", destination, source);
  
  nnz       = source->nnz;
  values    = source->values;
  
  base      = STORAGE_BASE(destination);
  r_encoder = base->callbacks->index_r_encoder;
  c_encoder = base->callbacks->index_c_encoder;
  
  s         = STORAGE_COORIDINATE(source);
  d         = STORAGE_COMPRESSED(destination);
  tuples    = s->tuples;
  R         = d->RO;
  C         = d->CO;
  K         = d->KO;
  
  qsort(tuples, nnz, sizeof(coordinate_tuple_t), base->callbacks->index_compare);
  tensor_storage_copy(d, s, nnz, base->callbacks->index_copy);
  tensor_storage_copy(destination, source, nnz, (index_copy_t) &copier_for_values);
  
  rn      = 0;
  cn      = 0;
  prev_ri = r_encoder(&tuples[0]);
  prev_ci = c_encoder(&tuples[0]);
  
  C[cn++] = 0;
  R[rn++] = 1;
  
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
      R[rn++] = cn;
      prev_ri = index;
    }
  }
  
  DEBUG("C[size=%u]=%u\n", cn, nnz);
  DEBUG("R[size=%u]=%u\n", rn, cn);
  
  C[cn++] = nnz;
  R[rn++] = cn;
  
  DEBUG("rn=%u\n", rn);
  DEBUG("cn=%u\n", cn);
  
  d->rn = rn;
  d->cn = cn;
}

tensor_storage_compressed_t*
tensor_storage_compressed_gundersen(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_compressed_t *storage;
  conversion_callbacks_t      *callbacks;
  
  superfluous("tensor_storage_compressed_gundersen(tensor=0x%x)\n", tensor);
  
  storage     = MALLOC(tensor_storage_compressed_t);
  storage->rn = tensor->n + 1;
  storage->cn = tensor->n * tensor->n + 1;
  storage->tn = tensor->n * tensor->n + 1;
  storage->kn = tensor->nnz;
  storage->RO = MALLOC_N(uint, storage->rn);
  storage->CO = MALLOC_N(uint, storage->cn);
  storage->TO = MALLOC_N(uint, storage->tn);
  storage->KO = MALLOC_N(uint, storage->kn);
  
  debug("tensor_storage_compressed_gundersen: rn=%d, cn=%d, tn=%d, kn=%d\n", 
	storage->rn, storage->cn, storage->tn, storage->kn);
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_c_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::tube:
    callbacks->index_compare   = (index_compare_t) &index_compare_jik;
    callbacks->index_r_encoder = &encoder_for_j;
    callbacks->index_c_encoder = &encoder_for_i;
    callbacks->index_copy      = (index_copy_t) &copier_for_k;
    break;
  default:
    die("tensor_storage_compressed_gundersen: "
	"unknown or unsupported orientation %d.\n", 
	tensor->orientation);
    break;
  }
  
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = callbacks;
  
  superfluous("tensor_storage_compressed_gundersen: callbacks=0x%x\n", callbacks);
  superfluous("tensor_storage_malloc_compressed: storage->CO=0x%x\n", storage->CO);
  superfluous("tensor_storage_malloc_compressed: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_compressed_gundersen: storage->size (of RO)=%d\n", storage->rn);
  superfluous("tensor_storage_compressed_gundersen: storage->RO=0x%x\n", storage->RO);
  superfluous("tensor_storage_compressed_gundersen: storage=0x%x\n", storage);
  
  return storage;
}
