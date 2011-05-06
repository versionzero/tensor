
#include "error.h"
#include "tensor.h"

void
matrix_clear_cooridinate_storage(cooridinate_storage_t *s)
{
  
}

void
matrix_clear_ekmr_storage(ekmr_storage_t *s)
{
  
}

void
tensor_clear(tensor_t *t)
{
  switch(t->strategy) {
  case cooridinate:
    matrix_clear_cooridinate_storage(COORIDINATE_DATA(t));
    break;
  case ekmr:
    matrix_clear_ekmr_storage(EKMR_DATA(t));
    break;
  default:
    die("Unknown storage strategy '%d'.\n", t->strategy);
  }
}

