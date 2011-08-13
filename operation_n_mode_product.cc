
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "tensor.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

/*
  Computing ($pT$):
  Let $\T \in R^{n\times n\times n}$ be a tensor.
  Let $\M \in R^{n\times n}$ be a matrix.
  Let $p \in R^{n}$ be a vector.
  for i = 1 to l do
    for j = 1 to m do 
      for k = 1 to m do
        M[i][j] += p[k] * T[i][j][k]
      end for
    end for
  end for
*/

extern cache_t *cache;

void
compressed_row(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r, r0, t, m, n;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C, *T;
  tensor_storage_compressed_t const *storage;
  
  debug("compressed_row(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->rn;
  R       = storage->RO;
  C       = storage->CO;
  T       = storage->KO;
  
  /*
    Using \emph{compressed row storage} ($\CRS$), this tensor can be
    represented as:
    
           $k$   0   1   2    3   4   5   6    7   8   9   10   11
     $\rowcrs$ & 0 & 4 & 8 & 12
     $\colcrs$ & 1 & 3 & 0 &  2 & 0 & 2 & 1 &  2 & 1 & 2 &  0 &  3
    $\tubecrs$ & 0 & 0 & 1 &  1 & 0 & 0 & 1 &  1 & 0 & 0 &  1 &  1
     $\valcrs$ & 1 & 2 & 7 &  8 & 3 & 4 & 9 & 10 & 5 & 6 & 11 & 12
  */
  
  DEBUG("\n");
  
  for (r = 1; r < size; ++r) {
    r0    = r-1;
    i     = r0;
    start = R[r0];
    end   = R[r];
    
    cache_access(cache, &R[r0], cache_operation::read);
    cache_access(cache, &R[r],  cache_operation::read);
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      j = c;
      t = T[k];
      
      cache_access(cache, &C[k], cache_operation::read);
      cache_access(cache, &T[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[t=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], t, p[t], k, V[k], p[t] * V[k]);
      
      M[i][j] += p[t] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d, T[k]=%d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, C[k], T[k], c, r, t, i, j);
    }
  }
}

void
compressed_tube(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r, r0, t, m, n;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C, *T;
  tensor_storage_compressed_t const *storage;
  
  debug("compressed_row(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->rn;
  R       = storage->RO;
  C       = storage->CO;
  T       = storage->KO;
  
  /*
    Using \emph{compressed row storage} ($\CRS$), this tensor can be
    represented as:
    
           $k$   0   1   2    3   4   5   6    7   8   9   10   11
     $\rowcrs$ & 0 & 4 & 8 & 12
     $\colcrs$ & 1 & 3 & 0 &  2 & 0 & 2 & 1 &  2 & 1 & 2 &  0 &  3
    $\tubecrs$ & 0 & 0 & 1 &  1 & 0 & 0 & 1 &  1 & 0 & 0 &  1 &  1
     $\valcrs$ & 1 & 2 & 7 &  8 & 3 & 4 & 9 & 10 & 5 & 6 & 11 & 12
  */
  
  DEBUG("\n");
  
  for (r = 1; r < size; ++r) {
    r0    = r-1;
    i     = r0;
    start = R[r0];
    end   = R[r];
    
    cache_access(cache, &R[r0], cache_operation::read);
    cache_access(cache, &R[r],  cache_operation::read);
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      t = T[k]; // row
      j = t;
      
      cache_access(cache, &C[k], cache_operation::read);
      cache_access(cache, &T[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[c=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], c, p[c], k, V[k], p[r] * V[k]);
      
      M[i][j] += p[c] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d, T[k]=%d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, C[k], T[k], c, r, t, i, j);
    }
  }
}

void
n_mode_product_compressed(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("n_mode_product_compressed(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  switch (tensor->orientation) {
  case orientation::row:
    compressed_row(matrix, vector, tensor);
    break;
  case orientation::tube:
    compressed_tube(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' orientation is not currently supported.\n",
	orientation_to_string(tensor->orientation));
    break;
  }
}

void
horizontal_slice(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r, r0, t, m, n;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C;
  tensor_storage_compressed_t const *storage;
  
  debug("horizontal_slice(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->rn;
  R       = storage->RO;
  C       = storage->CO;
  
  DEBUG("\nsize=%d, m=%d, n=%d\n", size, m, n);
  
  for (r = 1; r < size; ++r) {
    r0    = r-1;
    i     = r0 / n;
    t     = r0 % n;
    start = R[r0];
    end   = R[r];
    
    cache_access(cache, &R[r0], cache_operation::read);
    cache_access(cache, &R[r],  cache_operation::read);
    
    DEBUG("r0=%d, start=%d, end=%d\n", r0, start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      j = c;
      
      cache_access(cache, &C[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += ", i, j, M[i][j]);
      DEBUG("(p[t=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", t, p[t], k, V[k], p[t] * V[k]);
      
      M[i][j] += p[t] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, C[k], c, r, t, i, j);
    }
  }
}

void
lateral_slice(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r, r0, t, m, n;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C;
  tensor_storage_compressed_t const *storage;
  
  debug("lateral_slice(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->rn;
  R       = storage->RO;
  C       = storage->CO;
  
  DEBUG("\nsize=%d, m=%d, n=%d\n", size, m, n);
  
  for (r = 1; r < size; ++r) {
    r0    = r-1;
    start = R[r0];
    end   = R[r];
    
    cache_access(cache, &R[r0], cache_operation::read);
    cache_access(cache, &R[r],  cache_operation::read);
    
    DEBUG("r0=%d, start=%d, end=%d\n", r0, start, end);
    
    for (k = start; k < end; ++k) {
      j = k / n;
      t = k % n;
      c = C[k];
      i = c;
      
      cache_access(cache, &C[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += ", i, j, M[i][j]);
      DEBUG("(p[t=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", t, p[t], k, V[k], p[t] * V[k]);
      
      M[i][j] += p[t] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, C[k], c, r, t, i, j);
    }
  }
}

void
frontal_slice(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r, r0, t, m, n;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C;
  tensor_storage_compressed_t const *storage;
  
  debug("frontal_slice(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->rn;
  R       = storage->RO;
  C       = storage->CO;
  
  /*
    Using frontal \emph{compressed slice storage} ($\FCSS$), this
    tensor can be represented as:
    
           $k$   0   1   2    3   4   5   6    7   8   9   10   11  12
     $\rowcss$   0   1   3    4   6   7   9   10  12  13
      $\ctcss$   1   0   2    1   0   2   1    0   2   1    0    2   1
     $\valcss$   1   2   3    4   5   6   7    8   9  10   11   12  13
     
  */
  
  DEBUG("\nsize=%d, m=%d, n=%d\n", size, m, n);
  
  for (r = 1; r < size; ++r) {
    r0    = r-1;
    i     = r0 % n;
    t     = r0 / n;
    start = R[r0];
    end   = R[r];
    
    cache_access(cache, &R[r0], cache_operation::read);
    cache_access(cache, &R[r],  cache_operation::read);
    
    DEBUG("r0=%d, start=%d, end=%d\n", r0, start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      j = c;
      
      cache_access(cache, &C[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += ", i, j, M[i][j]);
      DEBUG("(p[t=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", t, p[t], k, V[k], p[t] * V[k]);
      
      M[i][j] += p[t] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, C[k], c, r, t, i, j);
    }
  }
}

void
n_mode_product_compressed_slice(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("n_mode_product_compressed_slice(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  switch (tensor->orientation) {
  case orientation::horizontal:
    horizontal_slice(matrix, vector, tensor);
    break;
  case orientation::lateral:
    lateral_slice(matrix, vector, tensor);
    break;
  case orientation::frontal:
    frontal_slice(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' orientation is not currently supported.\n",
	orientation_to_string(tensor->orientation));
    break;
  }
}

void
ekmr_row(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                 i, j, k;
  uint                 size, nnz;
  uint                 start, end;
  uint                 c, ck, r, r0, t, m, n;
  double               **M;
  double const         *p, *V;
  uint const           *R, *CK;
  tensor_storage_extended_t const *storage;
  
  debug("ekmr_row(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  m       = matrix->m;
  n       = matrix->n;
  
  storage = STORAGE_EXTENDED(tensor);
  size    = storage->size;
  R       = storage->RO;
  CK      = storage->CK;
  
  /* 
     Now, using \emph{extended compressed row storage} ($\ECRS$), the
     original tensor can be represented as:
     
           $k$   0   1   2    3   4   5   6    7   8   9   10   11
     $\rowcrs$ & 0 & 4 & 8 & 12
      $\ctcrs$ & 1 & 2 & 5 &  6 & 0 & 3 & 4 &  5 &  1 & 2 & 4 &  7
     $\valcrs$ & 7 & 1 & 8 &  2 & 3 & 9 & 4 & 10 & 11 & 5 & 6 & 12
  */
  
  DEBUG("\n");
  
  for (r = 1; r < size; ++r) {
    r0    = r-1;
    i     = r0;
    start = R[r0];
    end   = R[r];
    
    cache_access(cache, &R[r0], cache_operation::read);
    cache_access(cache, &R[r], cache_operation::read);
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      ck = CK[k];
      c  = ck / n;
      j  = c;
      t  = ck % n;
      
      cache_access(cache, &CK[k], cache_operation::read);
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[t=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], t, p[t], k, V[k], p[j] * V[k]);
      
      M[i][j] += p[t] * V[k];
      
      cache_access(cache, &V[k],    cache_operation::read);
      cache_access(cache, &p[t],    cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::read);
      cache_access(cache, &M[i][j], cache_operation::write);
      
      cache_debug(cache);
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("CK[k=%2d]=%2d => c=%d, r=%d, t=%d, i=%d, j=%d\n", k, CK[k], c, r, t, i, j);
    }
  }
}

void
n_mode_product_ekmr(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("n_mode_product_ekmr(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  switch (tensor->orientation) {
  case orientation::row:
    ekmr_row(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' orientation is not currently supported.\n",
	orientation_to_string(tensor->orientation));
    break;
  }
}

void
operation_n_mode_product_inplace(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("operation_n_mode_product_inplace(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::compressed:
    n_mode_product_compressed(matrix, vector, tensor);
    break;
  case strategy::slice:
    n_mode_product_compressed_slice(matrix, vector, tensor);
    break;
  case strategy::ekmr:
  case strategy::zzekmr:  /* NOTE: the encoding may differ, but the
			     way we calculate products remains the
			     same.  How is that for simplicity? */
    n_mode_product_ekmr(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' strategy is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
}

matrix_t*
operation_n_mode_product(vector_t const *vector, tensor_t const *tensor)
{
  matrix_t *matrix;
  
  compatible(vector, tensor);
  debug("operation_n_mode_product(vector=0x%x, tensor=0x%x)\n", vector, tensor);
  
  matrix = matrix_malloc(tensor->m, tensor->n, ownership::creator);
  debug("operation_n_mode_product: matrix=0x%x\n", matrix);
 
  operation_n_mode_product_inplace(matrix, vector, tensor);
  
  return matrix;
}
