
#include "utility.h"
#include <stdlib.h>

void
safe_delete(void *p)
{
  free(p);
  p = NULL;
}

