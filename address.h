
#ifndef _MEMORY_ADDRESS_
#define _MEMORY_ADDRESS_

#include "types.h"

size_t memory_address_hash(void const *address);
size_t memory_address_tag(void const *address);
size_t memory_address_compare(void const *a, void const *b);
void*  memory_address_duplicate(void const *address);
void   memory_address_free(void *address);

#endif /* _MEMORY_ADDRESS_ */

/*
  Local Variables:
  mode: C++
  End:
*/
