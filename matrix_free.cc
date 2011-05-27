
#include "error.h"
#include "matrix.h"
#include "memory.h"
#include "utility.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void
matrix_free(matrix_t *t)
{
  uint i;
  
  debug("matrix_free(matrix_t=0x%x)\n", t);
  assert(NULL != t);
  
  if (creator == t->owner) {
    
    assert(NULL != t->data);
    
    for (i = 0; i < t->m; ++i) {
      assert(NULL != t->data[i]);
      safe_free(t->data[i]);
    }
    
    safe_free(t->data);
  }
  
  if (NULL != t) {
    safe_free(t);
  }
}

