
#include "error.h"
#include "tensor.h"

void
matrix_clear_coordinate_storage(coordinate_storage_t *s)
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
  case coordinate:
    matrix_clear_coordinate_storage(COORIDINATE_STORAGE(t));
    break;
  case ekmr:
    matrix_clear_ekmr_storage(EKMR_STORAGE(t));
    break;
  default:
    die("Unknown storage strategy '%d'.\n", t->strategy);
  }
}

