
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

void
matrix_copy_shallow_inplace(matrix_t *mr, matrix_t const *m1)
{
  error(D_INFORMATION, "Matrix Copy (inplace, shallow) (%d, %d) => (%d, %d)\n",
	mr->m, mr->n, m1->m, m1->n);
  
  mr->owner = VIEWER;
  mr->data  = m1->data;
}

matrix_t*
matrix_copy_shallow(matrix_t const *m1)
{
  matrix_t *mr;
  
  error(D_INFORMATION, "Matrix Copy (shallow) (%d, %d)\n",
	m1->m, m1->n);
  
  if (NULL == (mr = matrix_new(m1->m, m1->n, VIEWER))) {
    error(D_ERROR, "Failed to allocate matrix.\n");
  }
  
  matrix_copy_shallow_inplace(mr, m1);

  return mr;
}

void
matrix_copy_inplace_with_offset(matrix_t *mr, uint oi, uint oj, matrix_t const *m1)
{
  uint i, j;
  
  error(D_INFORMATION, "Matrix Copy (inplace, with offset) (%d, %d) => (%d, %d)\n",
	mr->m, mr->n, m1->m, m1->n);
  
  matrix_check_range(mr, oi, oj, m1->m+oi, m1->n+oj);
  
  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      mr->data[oi+i][oj+j] = m1->data[i][j];
    }
  }
}

void
matrix_copy_inplace(matrix_t *mr, matrix_t const *m1)
{
  uint i, j;
  
  error(D_INFORMATION, "Matrix Copy (inplace) (%d, %d) => (%d, %d)\n",
	mr->m, mr->n, m1->m, m1->n);
  
  matrix_compatible(mr, m1);
  
  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      mr->data[i][j] = m1->data[i][j];
    }
  }
}

matrix_t*
matrix_copy(matrix_t const *m1)
{
  matrix_t *mr;
  
  error(D_INFORMATION, "Matrix Copy (%d, %d)\n",
	m1->m, m1->n);
  
  if (NULL == (mr = matrix_new(m1->m, m1->n))) {
    error(D_ERROR, "Failed to allocate matrix.\n");
  }
  
  matrix_copy_inplace(mr, m1);
  
  return mr;
}
