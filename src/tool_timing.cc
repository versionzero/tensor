
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "operation.h"
#include "tensor.h"
#include "tool.h"
#include "utility.h"
#include "vector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

vector_t*
timed_vector_read(char const *name)
{
  clock_t  t;
  vector_t *vector;
  
  message("Reading vector %s ... ", name);
  t = clock();
  vector = vector_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));

  return vector;
}

matrix_t*
timed_matrix_read(char const *name)
{
  clock_t  t;
  matrix_t *matrix;
  
  message("Reading matrix %s ... ", name);
  t = clock();
  matrix = matrix_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  return matrix;
}

tensor_t*
timed_tensor_read(char const *name)
{
  clock_t  t;
  tensor_t *tensor;
  
  message("Reading tensor %s ... ", name);
  t = clock();
  tensor = tensor_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  return tensor;
}
