
#include "error.h"
#include "memory.h"
#include "matrix.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

typedef uint (*slice_distance_t)(tensor_t *tensor, uint i, uint j);

uint
slice_distance_for_compressed_tube(tensor_t *tensor, uint s1, uint s2)
{
  uint                              i, j, c1, c2, k1, k2;
  uint                              distance;
  uint const                        *R, *C, *K;
  tensor_storage_compressed_t const *storage;
  
  superfluous("slice_distance_for_compressed_tube(vector=0x%x, s1=%d, s2=%d)\n", tensor, s1, s2);
  
  distance = 0;
  storage  = STORAGE_COMPRESSED(tensor);
  R        = storage->RO;
  C        = storage->CO;
  K        = storage->KO;
  
  for (i = R[s1], j = R[s2]; i < R[s1+1] && j < R[s2+1];) {
    c1 = C[i]; k1 = K[i]; 
    c2 = C[j]; k2 = K[j];
    DEBUG("%d ?= %d == %s, %d ?= %d == %s\n",
	  c1, c2, c1 == c2 ? "YES" : "NO",
	  k1, k2, k1 == k2 ? "YES" : "NO");
    if (c1 != c2 || k1 != k2) {
      distance++;
      if (c1 < c2 || k1 < k2) {
	i++;
      } else if (c2 < c1 || k2 < k1) {
	j++;
      }
    } else {
      i++; j++;
    }
  }
  
  DEBUG("slice_distance_for_compressed_tube: distance=%d\n", distance);
  
  return distance;
}

void
tensor_find_slice_permutation_inplace(vector_t *vector, tensor_t *tensor, permutation_heuristic::type_t heuristic, slice_distance_t distance)
{
  uint     i, j;
  uint     n;
  uint     best, si, sj;
  matrix_t *matrix;
  double   **M;
  
  debug("tensor_find_slice_permutation_inplace(vector=0x%x, tensor=0x%x, heuristic='%s', distance=0x%x)\n", 
	vector, tensor, permutation_heuristic_to_string(heuristic), distance);
  
  n      = tensor->n;
  matrix = matrix_malloc(n, n);
  M      = matrix->data;
  
  best   = n*n+1;
  xi     = 0;
  xj     = 0;
  
  matrix_clear(matrix);
  
  for (i = 0; i < n; ++i) {
    for (j = i+1; j < n; ++j) {
      M[i][j] = (*distance)(tensor, i, j);
    }
  }
  
  matrix_fwrite(stdout, matrix, format::coordinate);
}

void
permutation_supported(tensor_t *tensor)
{
  debug("permutation_supported(tensor=0x%x)\n", tensor);
  
  if (tensor->strategy != strategy::compressed && tensor->orientation != orientation::tube) {
    die("permutation_supported: the combination of tensor strategy '%s' and '%s' orientation is not supported.\n", 
	strategy_to_string(tensor->strategy), orientation_to_string(tensor->orientation));
  }
}

void
tensor_find_permutation_inplace(vector_t *vector, tensor_t *tensor, permutation_heuristic::type_t heuristic)
{
  slice_distance_t distance;
  
  debug("tensor_find_permutation_inplace(vector=0x%x, tensor=0x%x, heuristic='%s')\n",
	vector, tensor, permutation_heuristic_to_string(heuristic));
  
  permutation_supported(tensor);
  
  distance = &slice_distance_for_compressed_tube;
  
  tensor_find_slice_permutation_inplace(vector, tensor, heuristic, distance);
}

vector_t*
tensor_find_permutation(tensor_t *tensor, permutation_heuristic::type_t heuristic)
{
  vector_t *vector;
  
  debug("tensor_find_permutation(tensor=0x%x, heuristic='%s')\n",
	tensor, permutation_heuristic_to_string(heuristic));
  
  vector = vector_malloc(tensor->n);
  tensor_find_permutation_inplace(vector, tensor, heuristic);
  
  return vector;
}
