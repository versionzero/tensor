
#include "error.h"
#include "matrix.h"

void
matrix_compatible(matrix_t const *m1, matrix_t const *m2)
{
  error(D_INFORMATION, "Checking compatability (%d, %d) => (%d, %d)\n",
	m1->m, m1->n, m2->m, m2->n);

  if (m1->m != m2->m || m1->n != m2->n) {
    die( "Input matrices are not of the same size.\n");
  }
}

