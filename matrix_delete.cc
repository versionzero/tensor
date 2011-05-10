
#include "matrix.h"
#include "memory.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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
    safe_delete(t->data);
  }  
  if (NULL != t) {
    safe_delete(t);
  }
}

