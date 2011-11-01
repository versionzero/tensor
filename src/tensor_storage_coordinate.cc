
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

void
tensor_storage_convert_from_compressed_tube_to_coordinate(tensor_t *destination, tensor_t *source)
{
  uint                        i, t, r0, r;
  uint                        n, rn, nnz;
  tensor_storage_coordinate_t *d;
  tensor_storage_compressed_t *s;
  coordinate_tuple_t          *T;
  double                      *V;
  uint                        *R, *C, *K;
  
  s = STORAGE_COMPRESSED(source);
  d = STORAGE_COORIDINATE(destination);
  
  debug("tensor_storage_convert_from_compressed_tube_to_coordinate(destination=0x%x, source=0x%x)\n", destination, source);
  
  nnz = source->nnz;
  T   = d->tuples;
  
  n   = source->n;
  rn  = s->rn;
  R   = s->RO;
  C   = s->CO;
  K   = s->KO;
  V   = source->values;
  
  for (r = 1, t = 0; r < rn; ++r) {
    r0 = r-1;
    for (i = R[r0]; i < R[r]; ++i, ++t) {
      T[t].i     = r0;
      T[t].j     = K[i];
      T[t].k     = C[i];
      T[t].index = i;
      
    }
  }
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = source->values[i];
  }
}

void
tensor_storage_convert_from_compressed_slice_to_coordinate(tensor_t *destination, tensor_t *source)
{
  uint                        i, t, r0, r;
  uint                        n, rn, nnz;
  tensor_storage_coordinate_t *d;
  tensor_storage_compressed_t *s;
  coordinate_tuple_t          *T;
  double                      *V;
  uint                        *R, *C, *K;
  
  s = STORAGE_COMPRESSED(source);
  d = STORAGE_COORIDINATE(destination);
  
  debug("tensor_storage_convert_from_compressed_slice_to_coordinate(destination=0x%x, source=0x%x)\n", destination, source);
  
  nnz = source->nnz;
  T   = d->tuples;
  
  n   = source->n;
  rn  = s->rn;
  R   = s->RO;
  C   = s->CO;
  K   = s->KO;
  V   = source->values;
  
  for (r = 1, t = 0; r < rn; ++r) {
    r0 = r-1;
    DEBUG("R[r0=%u]=%u, R[r=%u]=%u\n", r0, R[r0], r, R[r]);
    for (i = R[r0]; i < R[r]; ++i, ++t) {
      DEBUG("K[i=%u]=%u\n", i, K[i]);
      T[t].i     = K[i] / n;
      T[t].j     = K[i] % n;
      T[t].k     = r0 % n;
      T[t].index = i;
      DEBUG("i=%u, j=%u, k=%u, index=%u\n", T[t].i, T[t].j, T[t].k, T[t].index);
    }
  }
  
  for (i = 0; i < nnz; ++i) {
    destination->values[i] = source->values[i];
  }
}

void
tensor_storage_convert_from_compressed_to_coordinate(tensor_t *destination, tensor_t *source)
{
  debug("tensor_storage_convert_from_compressed_to_coordinate(destination=0x%x, source=0x%x)\n", destination, source);
  
  switch (source->strategy) {
  case strategy::compressed:
    tensor_storage_convert_from_compressed_tube_to_coordinate(destination, source);
    break;
  case strategy::slice:
    tensor_storage_convert_from_compressed_slice_to_coordinate(destination, source);
    break;
  default:
    die("tensor_storage_convert_from_compressed_to_coordinate: "
	"unknown or unsupported strategy %d.\n", 
	source->strategy);
    break;
  }
}

tensor_storage_coordinate_t*
tensor_storage_malloc_coordinate(tensor_t const *tensor)
{
  tensor_storage_base_t       *base;
  tensor_storage_coordinate_t *storage;
  
  superfluous("tensor_storage_malloc_coordinate(tensor=0x%x, nnz=%d)\n", tensor, tensor->nnz);
  
  storage         = MALLOC(tensor_storage_coordinate_t);
  storage->tuples = MALLOC_N(coordinate_tuple_t, tensor->nnz);
  base            = (tensor_storage_base_t*) storage;
  base->callbacks = NULL;
  
  superfluous("tensor_storage_malloc_coordinate: storage->tuples=0x%x\n", storage->tuples);
  superfluous("tensor_storage_malloc_coordinate: storage=0x%x\n", storage);
  
  return storage;
}
