
#include "error.h"
#include "storage.h"
#include <string.h>

int
index_compare_ijk(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb)
{
  int result;
  
  if (0 == (result = ta->i - tb->i)) {
    if (0 == (result = ta->j - tb->j)) {
      result = ta->k - tb->k;
    }
  }
  
  return result;
}

int
index_compare_jik(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb)
{
  int result;
  
  if (0 == (result = ta->j - tb->j)) {
    if (0 == (result = ta->i - tb->i)) {
      result = ta->k - tb->k;
    }
  }
  
  return result;
}

int
index_compare_jki(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb)
{
  int result;
  
  if (0 == (result = ta->j - tb->j)) {
    if (0 == (result = ta->k - tb->k)) {
      result = ta->i - tb->i;
    }
  }
  
  return result;
}

int 
index_compare_kji(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb)
{
  int result;
  
  if (0 == (result = ta->k - tb->k)) {
    if (0 == (result = ta->j - tb->j)) {
      result = ta->i - tb->i;
    }
  }
  
  return result;
}

int 
index_compare_kij(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb)
{
  int result;
  
  if (0 == (result = ta->k - tb->k)) {
    if (0 == (result = ta->i - tb->i)) {
      result = ta->j - tb->j;
    }
  }
  
  return result;
}

int 
index_compare_ikj(coordinate_tuple_t const *ta, coordinate_tuple_t const *tb)
{
  int result;
  
  if (0 == (result = ta->i - tb->i)) {
    if (0 == (result = ta->k - tb->k)) {
      result = ta->j - tb->j;
    }
  }
  
  return result;
}


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
tensor_storage_index_encode(uint *indices, uint n, coordinate_tuple_t const *tuple, uint nnz, index_encoder_t encoder)
{
  uint i, t;
  uint index;
  
  debug("tensor_storage_index_encode(indices=0x%x, tuple=0x%x, nnz=%d)\n", indices, tuple, nnz);
  
#if 0
  for (current = 0; current < nnz; ++current) {
    DEBUG("current=%u: i=%u, j=%u, k=%u, index=%u\n", 
	  current, tuple[current].i, tuple[current].j,
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
#endif
  
  indices[0] = 0;
  index      = encoder(&tuple[0]);
  
  for (i = 1; i < index; ++i) {
    indices[i] = 0;
  }

  for (t = 0; t < nnz; ++t) {
    DEBUG("t=%u: i=%u, j=%u, k=%u, index=%u\n", t, tuple[t].i, tuple[t].j, tuple[t].k, tuple[t].index);
    index = encoder(&tuple[t]);
    if (i != index) {
      DEBUG("indices[i=%u]=%u\n", i, t);
      for (; i < index; ++i) {
	indices[i] = t;
      }
    }
  }
  
  for (; i < n; ++i) {
    indices[i] = nnz;
  }
  
  DEBUG("indices[i=%u]=%u\n", i, nnz);
  indices[i++] = nnz;
  DEBUG("i=%u\n", i);
  
  return i;
}

void
copier_for_i(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->KO[i] = source->tuples[i].i;
}

void
copier_for_j(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->KO[i] = source->tuples[i].j;
}

void
copier_for_k(tensor_storage_compressed_t *destination, tensor_storage_coordinate_t const *source, uint i)
{
  destination->KO[i] = source->tuples[i].k;
}

void
copier_for_values(tensor_t *destination, tensor_t const *source, uint i)
{
  destination->values[i] = source->values[STORAGE_COORIDINATE(source)->tuples[i].index];
}

void
tensor_storage_copy(void *destination, void const *source, uint nnz, index_copy_t copier)
{
  uint i;
  
  debug("storage_index_copy(destination=0x%x, source=0x%x)\n", destination, source);
  
  for (i = 0; i < nnz; ++i) {
    copier(destination, source, i);
  }
}
