
#include "error.h"
#include "tensor.h"

bool
is_power_of_two(unsigned int n)
{
  return (0 != n && (n & (n-1)));
}

void
tensor_supported(tensor_t const *t1)
{
  if (is_power_of_two(t1->l) && 
      is_power_of_two(t1->m) && 
      is_power_of_two(t1->n)) {
    die("Input tensor dimentions are not powers of two.\n");
  }
}

void 
tensor_check_range(tensor_t const *t1, uint i, uint j, uint k, uint m, uint n, int l)
{
  if (i > t1->m || m > t1->m ||
      j > t1->n || n > t1->n ||
      k > t1->l || l > t1->l) {
    die("Out of range %d < %d <= %d | %d < %d <= %d | %d < %d <= %d.\n", 
	k, l, t1->l, i, m, t1->m, j, n, t1->n);
  }
}
