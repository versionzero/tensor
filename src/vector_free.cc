
#include "error.h"
#include "vector.h"
#include "memory.h"
#include "utility.h"
#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void
vector_free(vector_t *v)
{
  debug("vector_free(vector=0x%x)\n", v);
  
  if (!v) {
    return;
  }
  
  if (ownership::creator == v->owner) {
    safe_free(v->data);
  }
  
  safe_free(v);
}

