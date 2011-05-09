
#include "error.h"
#include "tensor.h"
#include "utility.h"

#include <stdio.h>
#include <assert.h>

void
tensor_delete_cooridinate_storage(cooridinate_storage_t *s)
{
  safe_delete(s->I);
  safe_delete(s->J);
  safe_delete(s->K);
  safe_delete(s->values);
}

void
tensor_delete_ekmr_storage(ekmr_storage_t *s)
{
  safe_delete(s->I);
  safe_delete(s->J);
  safe_delete(s->values);
}

void
tensor_delete_storage(tensor_t *t)
{
  assert(NULL != t->storage);

  switch(t->strategy) {
  case cooridinate:
    tensor_delete_cooridinate_storage(COORIDINATE_STORAGE(t));
    break;
  case ekmr:
    tensor_delete_ekmr_storage(EKMR_STORAGE(t));
    break;
  default:
    die("Unknown storage strategy '%d'.\n", t->strategy);
  }

  safe_delete(t->storage);
}

void
tensor_delete(tensor_t *t)
{
  assert(NULL != t);

  if (creator == t->owner) {    
    tensor_delete_storage(t);    
  }

  if (NULL != t) {
    safe_delete(t);
  }
}

