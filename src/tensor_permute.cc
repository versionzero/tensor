
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
slice_distance(tensor_t *tensor, uint s1, uint s2)
{
  uint                              i, j, c1, c2, k1, k2, n;
  uint                              distance;
  uint const                        *R, *C, *K;
  tensor_storage_compressed_t const *storage;
  
  superfluous("slice_distance(vector=0x%x, s1=%d, s2=%d)\n", tensor, s1, s2);
  
  distance = 0;
  storage  = STORAGE_COMPRESSED(tensor);
  n        = storage->rn;
  R        = storage->RO;
  C        = storage->CO;
  K        = storage->KO;
  
  for (i = R[s1], j = R[s2]; i < R[s1+1] && j < R[s2+1];) {
#if 0
    c1 = C[i]; k1 = K[i]; 
    c2 = C[j]; k2 = K[j];
#endif
    c1 = K[i] / n; k1 = K[i] % n;
    c2 = K[j] / n; k2 = K[j] % n;
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
  
  DEBUG("slice_distance: distance(%d, %d)=%d\n", s1, s2, distance);
  
  return distance;
}


void
naive_minimum_permutation(vector_t *vector, tensor_t *tensor, slice_distance_t distance)
{
  uint     i, j, k;
  uint     n;
  uint     best;
  matrix_t *matrix;
  double   **D;
  uint     *V;
  bool     *seen;
  uint const                        *R, *C, *K;
  tensor_storage_compressed_t const *storage;
  
  debug("naive_minimum_permutation(vector=0x%x, tensor=0x%x, distance=0x%x)\n", 
	vector, tensor, distance);
  
  storage  = STORAGE_COMPRESSED(tensor);
  R        = storage->RO;
  C        = storage->CO;
  K        = storage->KO;
  
  n        = tensor->n;
  matrix   = matrix_malloc(n, n);
  D        = matrix->data;
  V        = vector->data;
    
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
	  DEBUG("permutation: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
  }
  
  DEBUG("permutation: best=%d, V[0]=%d, V[1]=%d\n", best, V[0], V[1]);
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
	DEBUG("permutation: looking-at(%d, %d)=%lf\n", i, j, D[i][j]);
	if (best > D[i][j]) {
	  best = D[i][j];
	  k    = i;
	  DEBUG("permutation: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
    V[j]       = k;
    seen[V[j]] = true;
    DEBUG("permutation: best=%d, V[%d]=%d, V[%d]=%d\n", best, j, V[j-1], j, V[j]);
    DEBUG("permutation: seen=%d\n", k);
  }
  
  safe_free(seen);
#if 0
  vector_fwrite(stdout, vector)
  vector_fwrite(stdout, mean);
#endif
}

/*
 * The following code is public domain.
 * Algorithm by Torben Mogensen, implementation by N. Devillard.
 * This code in public domain.
 *
 * Source: http://ndevilla.free.fr/median/median/src/
 */
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
naive_median_permutation(vector_t *vector, tensor_t *tensor, slice_distance_t distance)
{
  uint     i, j, k;
  uint     n;
  uint     best, difference;
  vector_t *mean;
  matrix_t *matrix;
  double   **D;
  uint     *V, *M;
  bool     *seen;
  
  debug("naive_median_permutation(vector=0x%x, tensor=0x%x, distance=0x%x)\n", 
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
	  DEBUG("permutation: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
  }
  
  for (i = 0; i < n; ++i) {
    M[i] = non_destructive_median(D[i], n, i);
  }
  
  DEBUG("permutation: best=%d, V[0]=%d, V[1]=%d\n", best, V[0], V[1]);
#if 0
  //matrix_fwrite(stdout, matrix, format::coordinate);
#endif
  
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
	DEBUG("permutation: looking-at(%d, %d)=%lf (difference=%lf)\n", i, j, D[i][j], difference);
	if (best > difference) {
	  best = difference;
	  k    = i;
	  DEBUG("permutation: best(%d, %d)=%d\n", i, j, best);
	}
      }
    }
    V[j]       = k;
    seen[V[j]] = true;
    DEBUG("permutation: best=%d, V[%d]=%d, V[%d]=%d\n", best, j, V[j-1], j, V[j]);
    DEBUG("permutation: seen=%d\n", k);
  }
  
  safe_free(seen);
  safe_free(matrix);
  
#if 0  
  vector_fwrite(stdout, mean);
  vector_fwrite(stdout, vector);
#endif
}



tensor_t*
tensor_apply_permutation(tensor_t *source, vector_t *vector)
{
  uint                        i, i1, i2, r0, r;
  uint                        n, rn, nnz, offset;
  uint const                  *R1, *K1, *V;
  uint                        *R2, *K2;
  tensor_storage_compressed_t *storage;
  tensor_t                    *destination;
  double                      *V1, *V2;
  
  superfluous("tensor_apply_permutation(vector=0x%x, vector=%0x%x)\n", source, vector);
  
  V           = vector->data;
  
  n           = source->n;
  nnz         = source->nnz;
  storage     = STORAGE_COMPRESSED(source);
  rn          = storage->rn;
  R1          = storage->RO;
  //C1          = storage->CO;
  K1          = storage->KO;
  V1          = source->values;
  
  destination = tensor_malloc(n, n, n, nnz, strategy::slice, orientation::frontal);
  storage     = STORAGE_COMPRESSED(destination);
  R2          = storage->RO;
  //C2          = storage->CO;
  K2          = storage->KO;
  V2          = destination->values;
  storage->rn = rn;
  
  offset      = 0;
  R2[0]       = 0;
  
  for (i = 0; i < n; ++i) {
    r0 = R1[V[i]];
    r  = R1[V[i]+1];
    DEBUG("> r0=R1[V[i=%d]  =%d]=%d\n", i, V[i], r0);
    DEBUG("> r =R1[V[i=%d]+1=%d]=%d\n", i, V[i]+1, r);
    for (i1 = r0, i2 = offset; i1 < r && i2 < nnz; ++i1, ++i2) {
      //C2[i2] = C1[i1];
      K2[i2] = K1[i1];
      V2[i2] = V1[i1];
      //DEBUG("C2[i2=%d]=%d; C1[i1=%d]=%d\n",   i2, C2[i2], i1, C1[i1]);
      DEBUG("K2[i2=%d]=%d; K1[i1=%d]=%d\n",   i2, K2[i2], i1, K1[i1]);
      DEBUG("V2[i2=%d]=%lf; V1[i1=%d]=%lf\n", i2, V2[i2], i1, V1[i1]);
    }
    R2[i+1] = offset + r - r0;
    DEBUG("< R2[i=%d]=%d+%d-%d=%d\n", i, offset, r, r0, R2[i+1]);
    offset = R2[i+1];
  }
  
  R2[i] = nnz;
  
#if 0
  tensor_fwrite(stdout, destination);
#endif
  
  return destination;
}


void
permutation_supported(tensor_t *tensor)
{
  debug("permutation_supported(tensor=0x%x)\n", tensor);
  
  if (tensor->strategy != strategy::compressed || tensor->orientation != orientation::tube) {
    die("permutation_supported: the combination of tensor strategy '%s' and '%s' orientation is not supported.\n", 
	strategy_to_string(tensor->strategy), orientation_to_string(tensor->orientation));
  }
}


tensor_t*
tensor_permute(tensor_t *tensor, permutation_heuristic::type_t heuristic)
{
  vector_t            *vector;
  tensor_t            *coordinate, *frontal, *permuted, *result;
  slice_distance_t    distance;
  slice_permutation_t permutation;
  
  debug("tensor_permute(tensor=0x%x, heuristic='%s')\n",
	tensor, permutation_heuristic_to_string(heuristic));
  
  permutation_supported(tensor);
  
  distance   = &slice_distance;
  vector     = vector_malloc(tensor->n);
  coordinate = tensor_convert(tensor, strategy::coordinate);
  frontal    = tensor_convert(coordinate, strategy::slice, orientation::frontal);
  
#if 0
  message("tube:\n");
  tensor_fwrite(stdout, tensor);
  message("coordinate:\n");
  tensor_fwrite(stdout, coordinate);
  message("compressed frontal slice:\n");
  tensor_fwrite(stdout, frontal);
#endif

  tensor_free(coordinate);
  
  switch (heuristic) {
  case permutation_heuristic::naive_minimum:
    permutation = &naive_minimum_permutation;
    break;
  case permutation_heuristic::naive_median:
    permutation = &naive_median_permutation;
    break;
  default:
    die("Heuristic '%d' is not supported.\n", heuristic);
    break;
  }
  
  (*permutation)(vector, frontal, distance);
  permuted   = tensor_apply_permutation(frontal, vector);
  
#if 0
  message("compressed frontal slice (permuted):\n");
  tensor_fwrite(stdout, permuted);
#endif
  
  coordinate = tensor_convert(permuted, strategy::coordinate);
  result     = tensor_convert(coordinate, tensor->strategy, tensor->orientation);
  
#if 0
  message("coordinate:\n");
  tensor_fwrite(stdout, coordinate);
  message("tube:\n");
  tensor_fwrite(stdout, result);
#endif

  tensor_free(permuted);
  tensor_free(coordinate);
  
  return result;
}

