
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
#include "timer.h"
#include <ctype.h>
#include <unistd.h>

vector_t*
timed_vector_read(char const *name)
{
  precision_timer_t  t;
  vector_t *vector;
  
  progress("Reading vector %s ... ", name);
  timer_start(&t);
  vector = vector_read(name);
  timer_end(&t);
  print_elapsed_time(t);

  return vector;
}

matrix_t*
timed_matrix_read(char const *name)
{
  precision_timer_t  t;
  matrix_t *matrix;
  
  progress("Reading matrix %s ... ", name);
  timer_start(&t);
  matrix = matrix_read(name);
  timer_end(&t);
  print_elapsed_time(t);
  
  return matrix;
}

tensor_t*
timed_tensor_read(char const *name)
{
  precision_timer_t  t;
  tensor_t *tensor;
  
  progress("Reading tensor %s ... ", name);
  timer_start(&t);
  tensor = tensor_read(name);
  timer_end(&t);
  print_elapsed_time(t);
  
  return tensor;
}

void
timed_tensor_fwrite(FILE *file, tensor_t const *tensor)
{
  precision_timer_t t;
  
  timer_start(&t);
  tensor_fwrite(file, tensor);
  timer_end(&t);
  print_elapsed_time(t);
}

void
timed_tensor_write(int argc, char *argv[], int const offset, tensor_t const *tensor)
{
  char     *name;
  FILE     *file;
  
  if (offset == argc) {
    file = stdout;
    progress("Writing tensor to stdout ...");
  } else {
    name = argv[offset];
    file = fopen_or_die(name, "w+");
    progress("Writing tensor to %s ... ", name);
  }
  
  timed_tensor_fwrite(file, tensor);
  
  if (stdout != file) {
    fclose(file);
  }
}
