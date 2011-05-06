
#include "error.h"
#include "tensor.h"

void
tensor_compatible(tensor_t const *t1, tensor_t const *t2)
{
  information("Checking tensor compatability\n");

  if (t1->l != t2->l || t1->m != t2->m || t1->n != t2->n) {
    die( "Input tensors do not have same dimensions.\n");
  }
}

