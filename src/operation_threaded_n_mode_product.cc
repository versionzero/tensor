
#include "cache.h"
#include "compatible.h"
#include "error.h"
#include "matrix.h"
#include "operation.h"
#include "tensor.h"
#include "utility.h"
#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

extern cache_t *cache;
extern uint    thread_count;

/*
  Computing ($pT$):
  Let $\T \in R^{n\times n\times n}$ be a tensor.
  Let $\M \in R^{n\times n}$ be a matrix.
  Let $p \in R^{n}$ be a vector.
  for i = 1 to l do
    for j = 1 to m do 
      for k = 1 to m do
        M[i][j] += p[k] * T[i][j][k]
      end for
    end for
  end for
*/

typedef struct {
  uint           done;
  matrix_t       *matrix;
  vector_t const *vector;
  tensor_t const *tensor;
} product_thread_data_t;

static pthread_mutex_t tube_lock;

int
next_tube(product_thread_data_t *p)
{
  uint k;
  
  pthread_mutex_lock(&tube_lock);
  k = p->done++;
  pthread_mutex_unlock(&tube_lock);
  return k < (p->tensor->n*p->tensor->n) ? k : -1;
}

void*
fiber_product(void *arg)
{
  int                   t;
  uint                  i, j, k, offset;
  uint                  n, sum;
  uint                  *P;
  double                **M, *T;
  product_thread_data_t *p;
  
  p = (product_thread_data_t*) arg;
  
  M = p->matrix->data;
  P = p->vector->data;
  T = p->tensor->values;
  
  n = p->tensor->n;
  
  while (-1 != (t = next_tube(p))) {
    sum    = 0;
    offset = t*n;
    i      = t/n;
    j      = t%n;
    for (k = 0; k < n; ++k) {
      sum += P[k] * T[offset+k];
    }
    M[i][j] = sum;
  }
  
  return NULL;
}

void
threaded_n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint                  i;
  pthread_t             threads[32];
  int                   error;
  product_thread_data_t data;
  
  data.done   = 0;
  data.matrix = matrix;
  data.vector = vector;
  data.tensor = tensor;
  
  pthread_mutex_init(&tube_lock, NULL);
  
  for (i = 0; i < thread_count; ++i) {
    if (0 != (error = pthread_create(&threads[i], NULL, fiber_product, &data))) {
      die("threaded_n_mode_product_array: pthread_create() failed with %d\n", error);
    }
  }
  
  for (i = 0; i < thread_count; ++i) {
    if (0 != (error = pthread_join(threads[i], NULL))) {
      die("threaded_n_mode_product_array: pthread_join() failed with %d\n", error);
    }
  }
  
  pthread_mutex_destroy(&tube_lock);
}

void
n_mode_product_array(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  uint   i, j, k, index;
  uint   n;
  uint   *P;
  double **M, *T;
  
  debug("n_mode_product_array(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  n = tensor->n;
  M = matrix->data;
  P = vector->data;
  T = tensor->values;
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      for (k = 0; k < n; ++k) {
	index = tensor_index(tensor, i, j, k);
	M[i][j] += P[k] * T[index];
      }
    }
  }
}

void
threaded_n_mode_product(matrix_t *matrix, vector_t const *vector, tensor_t const *tensor)
{
  debug("threaded_n_mode_product(matrix=0x%x, vector=0x%x, tensor=0x%x)\n", matrix, vector, tensor);
  
  compatible(vector, tensor);
  
  switch (tensor->strategy) {
  case strategy::array:
    threaded_n_mode_product_array(matrix, vector, tensor);
    break;
  default:
    die("Tensor product for '%s' strategy (using thread_count) is not currently supported.\n",
	strategy_to_string(tensor->strategy));
    break;
  }
}
