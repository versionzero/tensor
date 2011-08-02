
#include "error.h"
#include "storage.h"
#include <string.h>

uint
encoder_for_i(coordinate_tuple_t const *tuple)
{
  return tuple->i;
}

uint
encoder_for_j(coordinate_tuple_t const *tuple)
{
  return tuple->j;
}

uint
encoder_for_k(coordinate_tuple_t const *tuple)
{
  return tuple->k;
}

uint
tensor_storage_index_encode(uint *indices, void const *p, uint nnz, index_encoder_t encoder)
{
  uint size, index, current, previous;
  coordinate_tuple_t const *tuple; 
  
  tuple      = (coordinate_tuple_t const*) p;
  size       = 0;
  previous   = 0;
  
  debug("tensor_storage_index_encode(indices=0x%x, tuple=0x%x)\n", indices, tuple);
  
  indices[size++] = 0;
  for (current = 0; current < nnz; ++current) {
    DEBUG("i=%u, j=%u, k=%u, index=%u\n", 
	  tuple[current].i, tuple[current].j, 
	  tuple[current].k, tuple[current].index);
    index = encoder(&tuple[current]);
    if (previous != index) {
      DEBUG("indices[size=%u]=%u\n", size, current);
      indices[size++] = current;
      previous        = index;
    }
  }
  DEBUG("indices[size=%u]=%u\n", size, nnz);
  indices[size++] = nnz;
  DEBUG("size=%u\n", size);
  
  return size;
}
