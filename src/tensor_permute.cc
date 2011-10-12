
#include "error.h"
#include "memory.h"
#include "matrix.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

typedef uint (*slice_distance_t)(tensor_t *tensor, uint i, uint j);

bool
is_nonzero(tensor_t *tensor, uint cc, uint kk, uint slice)
{
  uint c, i, k;
  uint start, end;
  uint const                        *R, *C, *K;
  tensor_storage_compressed_t const *storage;
  
  storage = STORAGE_COMPRESSED(tensor);
  R       = storage->RO;
  C       = storage->CO;
  K       = storage->KO;
  
  start  = R[slice-1];
  end    = R[slice];
  
  for (i = start; i < end; ++k) {
      c = C[i];
      k = K[i];
      if (cc == c && kk == k) {
	return true;
      }
  }
  
  return false;
}

uint
slice_distance_for_compressed_tube(tensor_t *tensor, uint s1, uint s2)
{
  uint i, j;
  uint n;
  uint distance;
  
  superfluous("slice_distance_for_compressed_tube(vector=0x%x, i=%d, j=%d)\n", tensor, s1, s2);
  
  n        = tensor->n;
  distance = 0;
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      if (is_nonzero(tensor, i, j, s1) != is_nonzero(tensor, i, j, s2)) {
	distance++;
      }
    }
  }
  
  return distance;
}

void
tensor_find_slice_permutation_inplace(vector_t *vector, tensor_t *tensor, permutation_heuristic::type_t heuristic, slice_distance_t distance)
{
  uint     i, j;
  uint     n;
  matrix_t *matrix;
  double   **M;
  
  superfluous("tensor_find_slice_permutation_inplace(vector=0x%x, tensor=0x%x, heuristic='%s', distance=0x%x)\n", 
	      vector, tensor, permutation_heuristic_to_string(heuristic), distance);
  
  n      = tensor->n;
  matrix = matrix_malloc(n, n);
  M      = matrix->data;
  
  matrix_clear(matrix);
  
  for (i = 1; i < n; ++i) {
    for (j = i+1; j < n; ++j) {
      M[i][j] = (*distance)(tensor, i, j);
    }
  }
}

void
permutation_supported(tensor_t *tensor)
{
  superfluous("permutation_supported(tensor=0x%x)\n", tensor);
  
  if (tensor->strategy != strategy::compressed && tensor->orientation != orientation::tube) {
    die("permutation_supported: the combination of tensor strategy '%s' and '%s' orientation is not supported.\n", 
	strategy_to_string(tensor->strategy), orientation_to_string(tensor->orientation));
  }
}

void
tensor_find_permutation_inplace(vector_t *vector, tensor_t *tensor, permutation_heuristic::type_t heuristic)
{
  slice_distance_t distance;
  
  superfluous("tensor_find_permutation_inplace(vector=0x%x, tensor=0x%x, heuristic='%s')\n",
	      vector, tensor, permutation_heuristic_to_string(heuristic));
  
  permutation_supported(tensor);
  
  distance = &slice_distance_for_compressed_tube;
  
  tensor_find_slice_permutation_inplace(vector, tensor, heuristic, distance);
}

vector_t*
tensor_find_permutation(tensor_t *tensor, permutation_heuristic::type_t heuristic)
{
  vector_t *vector;
  
  superfluous("tensor_find_permutation(tensor=0x%x, heuristic='%s')\n",
	      tensor, permutation_heuristic_to_string(heuristic));
  
  vector = vector_malloc(tensor->n);
  tensor_find_permutation_inplace(vector, tensor, heuristic);
  
  return vector;
}
