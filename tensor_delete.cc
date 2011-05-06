
#include "matrix.h"

#include <stdio.h>
#include <assert.h>

void
matrix_delete_cooridinate_storage(cooridinate_storage_t *s)
{
  safe_delete(I);
  safe_delete(J);
  safe_delete(K);
  safe_delete(values);
}

void
matrix_delete_ekmr_storage(ekmr_storage_t *s)
{
  safe_delete(I);
  safe_delete(J);
  safe_delete(values);
}

void
matrix_delete_storage(matrix_t *t)
{
  switch(t->strategy) {
  case cooridinate:
    matrix_delete_cooridinate_storage(COORIDINATE_DATA(t));
    break;
  case ekmr:
    matrix_delete_ekmr_storage(EKMR_DATA(t));
    break;
  default:
    die("Unknown storage strategy '%d'.\n", t->strategy);
  }
}

void
matrix_delete(matrix_t *t)
{
  uint i;

  assert(NULL != t);
  if (creator == t->owner) {
    assert(NULL != t->data);
    for (i = 0; i < t->m; ++i) {
      assert(NULL != t->data[i]);
      safe_delete(t->data[i]);
    }
    tensor_delete_storage(t->data);
    safe_delete(t->data);
  }
  if (NULL != t) {
    safe_delete(t);
  }
}

