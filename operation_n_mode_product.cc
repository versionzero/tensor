
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

void
compressed_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor, uint m, uint n)
{
  uint                       i, j, k;
  uint                       size, nnz;
  uint                       start, end;
  uint                       c, r;
  double                     **M;
  double const               *p, *V;
  uint const                 *R, *C, *T;
  storage_compressed_t const *storage;
  
  debug("compressed_row_operation_product(matrix=0x%s, vector=0x%x, tensor=0%x", matrix, vector, tensor);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  
  storage = STORAGE_COMPRESSED(tensor);
  size    = storage->size;
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
  
  for (i = 0, r = 1; r < size; ++r, ++i) {
    start = R[i];
    end   = R[r];
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      c = C[k];
      j = T[k];
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[c=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], c, p[c], k, V[k], p[c] * V[k]);
      
      M[i][j] += p[c] * V[k];
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("C[k=%2d]=%2d, T[k]=%d => c=%d, i=%d, j=%d\n", k, C[k], T[k], c, i, j);
    }
  }
}

void
ekmr_row_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor, uint m, uint n)
{
  uint                 i, j, k;
  uint                 size, nnz, offset;
  uint                 start, end;
  uint                 c, r;
  double               **M;
  double const         *p, *V;
  uint const           *R, *CK;
  storage_extended_t const *storage;
  
  debug("ekmr_row_operation_n_mode_product(matrix=0x%s, vector=0x%x, tensor=0%x, l=%d, m=%d, n=%d)\n", matrix, vector, tensor, m, n);
  
  matrix_clear(matrix);
  
  p       = vector->data;
  M       = matrix->data;
  V       = tensor->values;
  nnz     = tensor->nnz;
  
  storage = STORAGE_EXTENDED(tensor);
  size    = storage->size;
  offset  = storage->r;
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
  
  for (i = 0, r = 1; r < size; ++r, ++i) {
    start = R[i];
    end   = R[r];
    
    DEBUG("start=%d, end=%d\n", start, end);
    
    for (k = start; k < end; ++k) {
      c = CK[k] / offset;
      j = CK[k] % offset;
      
      DEBUG("(M[i=%2d][j=%2d]=%2.0f += (p[c=%2d]=%2.0f * V[k=%2d]=%2.0f)=%2.0f)=", i, j, M[i][j], c, p[c], k, V[k], p[c] * V[k]);
      
      M[i][j] += p[c] * V[k];
      
      DEBUG("%2.0f\t", M[i][j]);
      DEBUG("CK[k=%2d]=%2d => c=%d, i=%d, j=%d\n", k, CK[k], c, i, j);
    }
  }
}

void
ekmr_operation_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor, uint m, uint n)
{
  debug("ekmr_operation_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x, l=%d, m=%d, n=%d)\n", matrix, vector, tensor, m, n);
  
  switch (tensor->orientation) {
  case orientation::row:
    ekmr_row_operation_n_mode_product(matrix, vector, tensor, m, n);
    break;
  default:
    die("Tensor product for '%s' orientation is not currently supported.\n",
	orientation_to_string(tensor->orientation));
    break;
  }
}

void
operation_n_mode_product_inplace(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor, uint m, uint n)
{
  debug("operation_n_mode_product_inplace(matrix=0x%x, vector=0x%x, tensor=0x%x, l=%d, m=%d, n=%d)\n", matrix, vector, tensor, m, n);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::compressed:
    compressed_operation_n_mode_product(matrix, vector, tensor, m, n);
    break;
  case strategy::ekmr:
    ekmr_operation_n_mode_product(matrix, vector, tensor, m, n);
    break;
  case strategy::zzekmr:
    /* NOTE: the encoding may differ, but the way we calculate
       products remains the same */
    ekmr_operation_n_mode_product(matrix, vector, tensor, m, n);
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
  uint     m, n;
  matrix_t *matrix;
  
  debug("operation_n_mode_product(vector=0x%x, tensor=0x%x)\n", vector, tensor);
  
  switch (tensor->orientation) {
  case orientation::row:
    /* rows * tubes */
    m = tensor->m;
    n = tensor->l;
    break;
  case orientation::column:
    /* columns * tubes */
    m = tensor->n;
    n = tensor->l;
    break;
  case orientation::tube:
    /* rows * columns */
    m = tensor->m;
    n = tensor->n;
    break;
  default:
    die("Unknown or unsupported orientation %d \n", tensor->orientation);
    break;
  }
  
  debug("operation_n_mode_product: m=%d, n=%d\n", m, n);
  
  compatible(vector, tensor);
  matrix = matrix_malloc(m, n);
  operation_n_mode_product_inplace(matrix, vector, tensor, m, n);
  
  return matrix;
}
