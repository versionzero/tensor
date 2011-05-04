
#include "matrix.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void
matrix_delete(matrix_t *t)
{
  uint i;

  assert(NULL != t);
  if (CREATOR == t->owner) {
    assert(NULL != t->data);
    for (i = 0; i < t->m; ++i) {
      assert(NULL != t->data[i]);
      free(t->data[i]);
      t->data[i] = NULL;
    }
    free(t->data);
    t->data = NULL;
  }
  if (NULL != t) {
    free(t);
    t = NULL;
  }
}

