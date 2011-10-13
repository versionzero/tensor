
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

extern bool human_readable;

void
print_elapsed_time(clock_t t)
{
  double elapsed;
  
  elapsed = SECONDS_SINCE(t);
  
  if (human_readable) {
    progress("done [%lf]\n", elapsed);
  } else {
    message("%lf ", elapsed);
  }
}

vector_t*
timed_vector_read(char const *name)
{
  clock_t  t;
  vector_t *vector;
  
  progress("Reading vector %s ... ", name);
  t = clock();
  vector = vector_read(name);
  print_elapsed_time(t);

  return vector;
}

matrix_t*
timed_matrix_read(char const *name)
{
  clock_t  t;
  matrix_t *matrix;
  
  progress("Reading matrix %s ... ", name);
  t = clock();
  matrix = matrix_read(name);
  print_elapsed_time(t);
  
  return matrix;
}

tensor_t*
timed_tensor_read(char const *name)
{
  clock_t  t;
  tensor_t *tensor;
  
  progress("Reading tensor %s ... ", name);
  t = clock();
  tensor = tensor_read(name);
  print_elapsed_time(t);
  
  return tensor;
}
