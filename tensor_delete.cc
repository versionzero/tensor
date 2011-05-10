
#include "error.h"
#include "memory.h"
#include "tensor.h"
#include "utility.h"
#include <stdio.h>
#include <assert.h>

void
tensor_delete_coordinate_storage(storage_coordinate_t *s)
{
  safe_delete(s->I);
  safe_delete(s->J);
  safe_delete(s->K);
  safe_delete(s->values);
}

void
tensor_delete_ekmr_storage(storage_ekmr_t *s)
{
  safe_delete(s->I);
  safe_delete(s->J);
  safe_delete(s->values);
}

void
tensor_delete_storage(tensor_t *t)
{
  if (NULL != t->storage) {
    switch(t->strategy) {
    case coordinate:
      tensor_delete_coordinate_storage(STORAGE_COORIDINATE(t));
      break;
    case ekmr:
      tensor_delete_ekmr_storage(STORAGE_EKMR(t));
      break;
    default:
      die("Unknown storage strategy '%d'.\n", t->strategy);
    }
  }

  safe_delete(t->storage);
}

void
tensor_delete(tensor_t *t)
{
  assert(NULL != t);

  tensor_delete_storage(t);
  safe_delete(t);
}

