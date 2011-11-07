
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
  superfluous("matrix_free(matrix=0x%x)\n", matrix);
  
  if (!matrix) {
    return;
  }
  
  if (ownership::creator == matrix->owner) {
    safe_free(matrix->data[0]);
    safe_free(matrix->data);
  }
  
  safe_free(matrix);
}

