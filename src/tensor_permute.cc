
#include "error.h"
#include "memory.h"
#include "matrix.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef uint (*slice_distance_t)(tensor_t *tensor, uint i, uint j);
typedef void (*slice_permutation_t)(vector_t *vector, tensor_t *tensor, slice_distance_t distance);

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
  
  DEBUG("slice_distance_for_compressed_tube: distance(%d, %d)=%d\n", s1, s2, distance);
  
  return distance;
}

uint
non_destructive_median(double m[], uint n, uint skip) 
{
  uint i, less, greater, equal;
  double min, max, guess, maxltguess, mingtguess;
  min = max = m[0] ;
  for (i=1 ; i<n ; i++) {
    if (i != skip) {
      if (m[i]<min) min=m[i];
      if (m[i]>max) max=m[i];
    }
  }
  while (1) {
    guess = (min+max)/2;
    less = 0; greater = 0; equal = 0; 
    maxltguess = min ;
    mingtguess = max ;
    for (i=0; i<n; i++) {
      if (i != skip) {
	if (m[i]<guess) {
	  less++;
	  if (m[i]>maxltguess) maxltguess = m[i] ; } 
	else if (m[i]>guess) {
	  greater++;
	  if (m[i]<mingtguess) mingtguess = m[i] ; } 
	else equal++;
      }
    }
    if (less <= (n+1)/2 && greater <= (n+1)/2) break ; 
    else if (less>greater) max = maxltguess ;
    else min = mingtguess;
  }
  if (less >= (n+1)/2) return maxltguess;
  else if (less+equal >= (n+1)/2) return guess; 
  else return mingtguess;
}

void
naive_median_permutation_inplace(vector_t *vector, tensor_t *tensor, slice_distance_t distance)
{
  uint     i, j, k;
  uint     n;
  uint     best, difference;
  matrix_t *matrix;
  vector_t *mean;
  double   **D;
  uint     *V, *M;
  bool     *seen;
  
  debug("naive_median_permutation_inplace(vector=0x%x, tensor=0x%x, distance=0x%x)\n", 
	vector, tensor, distance);
  
  n      = tensor->n;
  matrix = matrix_malloc(n, n);
  mean   = vector_malloc(n);
  D      = matrix->data;
  V      = vector->data;
  M      = mean->data;
  
  matrix_clear(matrix);
  
  for (j = 0; j < n; ++j) {
    best = n*n+1;
    for (i = 0; i < n; ++i) {
      if (i != j) {
	D[i][j] = (*distance)(tensor, i, j);
	if (best > D[i][j]) {
	  best = D[i][j];
	  V[0] = i;
	  V[1] = j;
	  DEBUG("permutation_inplace: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
  }
  
  for (i = 0; i < n; ++i) {
    M[i] = non_destructive_median(D[i], n, i);
  }
  
  DEBUG("permutation_inplace: best=%d, V[0]=%d, V[1]=%d\n", best, V[0], V[1]);
  //matrix_fwrite(stdout, matrix, format::coordinate);
  
  seen = MALLOC_N(bool, n);
  for (i = 0; i < n; ++i) {
    seen[i] = false;
  }
  
  seen[V[0]] = true;
  seen[V[1]] = true;
  
  for (j = 2; j < n; ++j) {
    best = n*n+1;
    k    = 0;
    for (i = 0; i < n; ++i) {
      if (!seen[i] && i != j) {
	difference = fabs(D[i][j]-M[i]);
	DEBUG("permutation_inplace: looking-at(%d, %d)=%lf (deviation=%lf)\n", i, j, D[i][j], fabs(D[i][j]-M[i]));
	if (best > difference) {
	  best = difference;
	  k    = i;
	  DEBUG("permutation_inplace: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
    V[j]       = k;
    seen[V[j]] = true;
    DEBUG("permutation_inplace: best=%d, V[%d]=%d, V[%d]=%d\n", best, j, V[j-1], j, V[j]);
    DEBUG("permutation_inplace: seen=%d\n", k);
  }
  
  safe_free(seen);
#if 0  
  vector_fwrite(stdout, mean);
  vector_fwrite(stdout, vector);
#endif
}

void
naive_minimum_permutation_inplace(vector_t *vector, tensor_t *tensor, slice_distance_t distance)
{
  uint     i, j, k;
  uint     n;
  uint     best;
  matrix_t *matrix;
  double   **D;
  uint     *V;
  bool     *seen;
  
  debug("naive_greedy_permutation_inplace(vector=0x%x, tensor=0x%x, distance=0x%x)\n", 
	vector, tensor, distance);
  
  n      = tensor->n;
  matrix = matrix_malloc(n, n);
  D      = matrix->data;
  V      = vector->data;
  
  matrix_clear(matrix);
  
  for (j = 0; j < n; ++j) {
    best = n*n+1;
    for (i = 0; i < n; ++i) {
      if (i != j) {
	D[i][j] = (*distance)(tensor, i, j);
	if (best > D[i][j]) {
	  best = D[i][j];
	  V[0] = i;
	  V[1] = j;
	  DEBUG("permutation_inplace: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
  }
  
  DEBUG("permutation_inplace: best=%d, V[0]=%d, V[1]=%d\n", best, V[0], V[1]);
  //matrix_fwrite(stdout, matrix, format::coordinate);
  
  seen = MALLOC_N(bool, n);
  for (i = 0; i < n; ++i) {
    seen[i] = false;
  }
  
  seen[V[0]] = true;
  seen[V[1]] = true;
  
  for (j = 2; j < n; ++j) {
    best = n*n+1;
    k    = 0;
    for (i = 0; i < n; ++i) {
      if (!seen[i] && i != j) {
	DEBUG("permutation_inplace: looking-at(%d, %d)=%lf (deviation=%lf)\n", i, j, D[i][j], fabs(D[i][j]-M[i]));
	if (best > D[i][j]) {
	  best = D[i][j];
	  k    = i;
	  DEBUG("permutation_inplace: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
    V[j]       = k;
    seen[V[j]] = true;
    DEBUG("permutation_inplace: best=%d, V[%d]=%d, V[%d]=%d\n", best, j, V[j-1], j, V[j]);
    DEBUG("permutation_inplace: seen=%d\n", k);
  }
  
  safe_free(seen);
#if 0  
  vector_fwrite(stdout, mean);
  vector_fwrite(stdout, vector);
#endif
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
  slice_distance_t    distance;
  slice_permutation_t permutation;
  
  debug("tensor_find_permutation_inplace(vector=0x%x, tensor=0x%x, heuristic='%s')\n",
	vector, tensor, permutation_heuristic_to_string(heuristic));
  
  permutation_supported(tensor);
  
  distance = &slice_distance_for_compressed_tube;
  
  switch (heuristic) {
  case permutation_heuristic::naive_minimum:
    permutation = &naive_minimum_permutation_inplace;
    break;
  case permutation_heuristic::naive_median:
    permutation = &naive_median_permutation_inplace;
    break;
  default:
    die("Heuristic '%d' is not supported.\n", heuristic);
    break;
  }
  
  (*permutation)(vector, tensor, distance);
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
