
#include "error.h"
#include "matrix.h"

bool
is_power_of_two(unsigned int n)
{
  return (0 != n && (n & (n-1)));
}

void
matrix_supported(matrix_t const *m1)
{
  if (is_power_of_two(m1->m) && is_power_of_two(m1->n)) {
    error(D_ERROR|D_FATAL, "Input matrices sizes are not powers of two.\n");
  }
}

void 
matrix_check_range(matrix_t const *m1, uint i, uint j, uint m, uint n)
{
  if (i > m1->m || m > m1->m || j > m1->n || n > m1->n) {
    error(D_ERROR|D_FATAL, 
          "Out of range %d < %d <= %d | %d < %d <= %d.\n",
          i, m, m1->m, 
          j, n, m1->n);
  }
}
