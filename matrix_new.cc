
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

matrix_t*
matrix_new(uint m, uint n, uint owner)
{
  uint     i;
  matrix_t *mr;

  if (NULL == (mr = (matrix_t*) malloc(sizeof(matrix_t)))) {
    error(D_ERROR, "Failed to allocate matrix.\n");
  }

  mr->m     = m;
  mr->n     = n;
  mr->data  = NULL;
  mr->owner = owner;
  
  if (VIEWER == owner) {
    return mr;
  }

  if (NULL == (mr->data = (double**) malloc(m*sizeof(double)))) {
    error(D_ERROR, "Failed to allocate matrix storage.\n");
  }
  
  for (i = 0; i < m; ++i) {
    if (NULL == (mr->data[i] = (double*) malloc(n*sizeof(double)))) {
      error(D_ERROR, "Failed to allocate row storage.\n");
    }
  }
  
  return mr;
}

