
#include "error.h"
#include "matrix.h"
#include "arithmetic.h"
#include <stdio.h>
#include <stdlib.h>

typedef double (*operation)(double, double);

void
matrix_operation(matrix_t       *m1, 
			 matrix_t const *m2, 
			 operation      op,
			 char const     *name)
{
  uint i, j;

  information("Operation '%s' (%d, %d) => (%d, %d)\n",
	name, m1->m, m1->n, m2->m, m2->n);
  
  matrix_compatible(m1, m2);

  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      m1->data[i][j] = op(m1->data[i][j], m2->data[i][j]);
    }
  }
}

matrix_t*
matrix_operation(matrix_t const *m1, 
		 matrix_t const *m2,
		 operation      op,
		 char const     *name)
{
  matrix_t *mr;

  information("Operation '%s' (%d, %d) => (%d, %d)\n",
	name, m1->m, m1->n, m2->m, m2->n);
  
  matrix_compatible(m1, m2);

  if (NULL == (mr = matrix_malloc(m1->m, m1->n))) {
    die("Failed to allocate output matrix.\n");
  }

  matrix_copy(mr, m1);
  matrix_operation(mr, m2, op, name);

  return mr;
}

#define MTX_OP(name)							\
  void matrix_##name(matrix_t *m1, matrix_t const *m2)			\
  { matrix_operation(m1, m2, name, #name);	}			\
  matrix_t* matrix_##name(matrix_t const *m1, matrix_t const *m2)	\
  { return matrix_operation(m1, m2, name, #name); }

MTX_OP(add)
MTX_OP(subtract)

void
matrix_multiply(matrix_t *mr, matrix_t const *m1, matrix_t const *m2)
{
  uint i, j, k;
  
  information("Operation 'Multiplication' (%d, %d) => (%d, %d)\n",
	m1->m, m1->n, m2->m, m2->n);
  
  matrix_compatible(m1, m2);
  
  for (i = 0; i < m1->m; ++i) {
    for (j = 0; j < m1->n; ++j) {
      mr->data[i][j] = 0.0;
      for (k = 0; k < m1->n; ++k) {
	mr->data[i][j] += m1->data[i][k] * m2->data[k][j];
      }
    }
  }
}

matrix_t*
matrix_multiply(matrix_t const *m1, matrix_t const *m2)
{
  matrix_t *mr;
  
  matrix_compatible(m1, m2);
  
  if (NULL == (mr = matrix_malloc(m1->m, m1->n))) {
    die("Failed to allocate output matrix.\n");
  }
  
  matrix_multiply(mr, m1, m2);

  return mr;
}

