
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

matrix_t*
matrix_new(uint m, uint n, ownership_t owner)
{
  uint     i;
  matrix_t *mr;

  if (NULL == (mr = (matrix_t*) malloc(sizeof(matrix_t)))) {
    die("Failed to allocate matrix.\n");
  }

  mr->m     = m;
  mr->n     = n;
  mr->data  = NULL;
  mr->owner = owner;
  
  if (viewer == owner) {
    return mr;
  }

  if (NULL == (mr->data = (double**) malloc(m*sizeof(double)))) {
    die("Failed to allocate matrix storage.\n");
  }
  
  for (i = 0; i < m; ++i) {
    if (NULL == (mr->data[i] = (double*) malloc(n*sizeof(double)))) {
      die("Failed to allocate row storage.\n");
    }
  }
  
  return mr;
}

matrix_t*
matrix_new_or_die(uint m, uint n, ownership_t owner)
{
  matrix_t *mr;

  if (NULL == (mr = matrix_new(m, n))) {
    die("Failed to allocate matrix.\n");
  }
  
  return mr;
}
