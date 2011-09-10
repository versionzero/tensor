
#include "matrix.h"

void
matrix_clear(matrix_t *m1)
{
  uint i, j;
  
  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      m1->data[i][j] = 0.0;
    }
  }
}

