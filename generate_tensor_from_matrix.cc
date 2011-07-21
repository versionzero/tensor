
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "random.h"
#include "tensor.h"
#include "tool.h"
#include "utility.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

/* Here we generate a tensor from a pattern matrix. For simplicity, we
   asume a square matrix. A pattern matrix is a matrix with 0/1
   entries.  The tensor is created (or induced) by projecting all 1
   entries in to the third dimention.  The projection is simple: we
   use the sparcity of the matrix to determine the sparcity of the
   induced tube, and randomly populate the tube with 0/1 values. Of
   course, we only actually store the 1 values, but we generate all of
   them. */

tensor_t*
generate_tensor_from_matrix(matrix_t *matrix)
{
  uint                        i, j, k;
  uint                        nnz, size, n;
  uint                        lower, upper;
  tensor_t                    *tensor;
  tensor_storage_coordinate_t *storage;
  coordinate_tuple_t          *tuples;
  double                      *values;
  double                      **data;
  
  debug("generate_tensor_from_matrix: matrix=0x%x\n", matrix);
  
  n     = matrix->n;
  upper = matrix->n*matrix->n;
  data  = matrix->data;
  nnz   = 0;
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      if (!might_as_well_be_zero(data[i][j])) {
	nnz++;
      }
    }
  }
  
  lower   = nnz;
  nnz    *= n;
  tensor  = tensor_malloc(n, n, n, nnz, strategy::coordinate);
  storage = STORAGE_COORIDINATE(tensor);
  tuples  = storage->tuples;
  size    = 0;
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      if (!might_as_well_be_zero(data[i][j])) {
	for (k = 0; k < n; ++k) {
	  if (lower == random_between(lower, upper)) {
	    tuples[size].i     = i;
	    tuples[size].j     = j;
	    tuples[size].k     = k;
	    tuples[size].index = size;
	    values[size]       = 1;
	    size++;
	  }
	  if (scheme::symmetric == matrix->scheme) {
	    if (lower == random_between(lower, upper)) {
	      tuples[size].i     = j;
	      tuples[size].j     = i;
	      tuples[size].k     = k;
	      tuples[size].index = size;
	      values[size]       = 1;
	      size++;
	    }
	  }
	}
      }
    }
  }
  
  return tensor;
}
