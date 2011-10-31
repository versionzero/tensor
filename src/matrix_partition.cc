
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

void
matrix_partition(matrix_t *mr, matrix_t const *m1, 
			 uint i, uint j, uint m, uint n)
{
  uint p, q;
  
  matrix_check_range(mr, 0, 0, m-i, n-j);
  matrix_check_range(m1, i, j, m, n);
  
  for (p = i; p < m; ++p) {
    for (q = j; q < n; ++q) {
      mr->data[p-i][q-j] = m1->data[p][q];
    }
  }
}

matrix_t*
matrix_partition(matrix_t const *m1, uint i, uint j, uint m, uint n)
{
  matrix_t *mr;
  
  matrix_check_range(m1, i, j, m, n);
  
  if (NULL == (mr = matrix_malloc(m-i, n-j))) {
    die( "Failed to allocate output matrix.\n");
  }
  
  matrix_partition(mr, m1, i, j, m, n);
    
  return mr;
}

