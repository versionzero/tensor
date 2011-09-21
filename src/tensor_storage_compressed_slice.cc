
#include "error.h"
#include "memory.h"
#include "mmio.h"
#include "tensor.h"
#include "storage.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>

static uint g_n;

void
copier_for_slice_lateral(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->KO[i] = source->tuples[i].i * g_n + source->tuples[i].k;
}

void
copier_for_slice_horizontal(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->KO[i] = source->tuples[i].j * g_n + source->tuples[i].k;
}

void
copier_for_slice_frontal(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->KO[i] = source->tuples[i].i * g_n + source->tuples[i].j;
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
  
  s = STORAGE_COORIDINATE(source);
  d = STORAGE_COMPRESSED(destination);
  
  debug("tensor_storage_convert_from_coordinate_to_compressed_slice_inplace(destination=0x%x, source=0x%x)\n", destination, source);

  base   = STORAGE_BASE(destination);
  nnz    = source->nnz;
  values = source->values;
  tuples = s->tuples;
  g_n    = source->n;
  
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
  storage->CO = NULL;
  storage->TO = NULL;
  storage->KO = MALLOC_N(uint, storage->kn);
  
  debug("tensor_storage_malloc_compressed_slice: rn=%d, kn=%d\n", storage->rn, storage->kn);
  
  callbacks                  = MALLOC(conversion_callbacks_t);
  callbacks->index_compare   = NULL;
  callbacks->index_r_encoder = NULL;
  callbacks->index_copy	     = NULL;
  
  switch (tensor->orientation) {
  case orientation::lateral:
    callbacks->index_compare   = (index_compare_t) &index_compare_jik;
    callbacks->index_r_encoder = &encoder_for_j;
    callbacks->index_copy      = (index_copy_t) &copier_for_slice_lateral;
    break;
  case orientation::horizontal:
    callbacks->index_compare   = (index_compare_t) &index_compare_ijk;
    callbacks->index_r_encoder = &encoder_for_i;
    callbacks->index_copy      = (index_copy_t) &copier_for_slice_horizontal;
    break;
  case orientation::frontal:
    callbacks->index_compare   = (index_compare_t) &index_compare_kij;
    callbacks->index_r_encoder = &encoder_for_k;
    callbacks->index_copy      = (index_copy_t) &copier_for_slice_frontal;
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
  superfluous("tensor_storage_malloc_compressed_slice: storage->TO=0x%x\n", storage->TO);
  superfluous("tensor_storage_malloc_compressed_slice: storage->KO=0x%x\n", storage->KO);
  superfluous("tensor_storage_malloc_compressed_slice: storage=0x%x\n", storage);
  
  return storage;
}
