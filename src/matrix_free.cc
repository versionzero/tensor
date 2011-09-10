
#include "error.h"
#include "matrix.h"
#include "memory.h"
#include "utility.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void
matrix_free(matrix_t *matrix)
{
  uint i;
  
  superfluous("matrix_free(matrix=0x%x)\n", matrix);
  
  if (!matrix) {
    return;
  }
  
  if (ownership::creator == matrix->owner) {
    for (i = 0; i < matrix->m; ++i) {
      safe_free(matrix->data[i]);
    }    
    safe_free(matrix->data);
  }
  
  safe_free(matrix);
}

