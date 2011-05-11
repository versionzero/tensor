
#include "error.h"
#include "tensor.h"

void
tensor_storage_clear_coordinate(storage_coordinate_t *s)
{
  int i;
  
  for (i = 0; i < s->nnz; ++i) {
    s->values[i] = 0.0;
    s->I[i] = s->J[i] = s->K[i] = 0;
  }
}

void
tensor_storage_clear_ekmr(storage_ekmr_t *s)
{
  
}

void
tensor_clear(tensor_t *t)
{
  switch(t->strategy) {
  case coordinate:
    tensor_storage_clear_coordinate(STORAGE_COORIDINATE(t));
    break;
  case ekmr:
    tensor_storage_clear_ekmr(STORAGE_EKMR(t));
    break;
  default:
    die("Unknown storage strategy '%d'.\n", t->strategy);
  }
}

