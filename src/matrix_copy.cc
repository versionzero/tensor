
#include "error.h"
#include "matrix.h"
#include "mmio.h"
#include <stdio.h>
#include <stdlib.h>

void
matrix_copy_shallow(matrix_t *mr, matrix_t const *m1)
{
  information("Matrix Copy (inplace, shallow)\n");
  
  mr->owner = ownership::viewer;
  mr->data  = m1->data;
}

matrix_t*
matrix_copy_shallow(matrix_t const *m1)
{
  matrix_t *mr;
  
  information("Matrix Copy (shallow)\n");
  mr = matrix_malloc(m1->m, m1->n, ownership::viewer);
  matrix_copy_shallow(mr, m1);

  return mr;
}

void
matrix_copy_with_offset(matrix_t *mr, uint oi, uint oj, matrix_t const *m1)
{
  uint i, j;
  
  information("Matrix Copy (inplace, with offset)");  
  matrix_check_range(mr, oi, oj, m1->m+oi, m1->n+oj);
  
  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      mr->data[oi+i][oj+j] = m1->data[i][j];
    }
  }
}

void
matrix_copy(matrix_t *mr, matrix_t const *m1)
{
  uint i, j;
  
  information("Matrix Copy (inplace)\n");  
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
  
  information("Matrix Copy\n");  
  mr = matrix_malloc(m1->m, m1->n);
  matrix_copy(mr, m1);
  
  return mr;
}
