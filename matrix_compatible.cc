
#include "error.h"
#include "matrix.h"

void
matrix_compatible(matrix_t const *m1, matrix_t const *m2)
{
  information("Checking matrix compatability\n");

  if (m1->m != m2->m || m1->n != m2->n) {
    die( "Input matrices do not have same dimensions.\n");
  }
}

