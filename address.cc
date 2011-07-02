
#include "address.h"

/* Source: http://www.concentric.net/~ttwang/tech/addrhash.htm
   
   2654435761 is the golden ratio of 2^32. The right shift of 3 bits
   assumes that all objects are aligned on the 8 byte boundary. If a
   system aligns on the 4 byte boundary, then a right shift of 2 bits
   should be done. 
*/
size_t
memory_address_hash(void const *address)
{
  register size_t key;
  
  key = (size_t) address;
  return (key >> 3) * 2654435761;
}

size_t
memory_address_compare(void const *a, void const *b)
{
  if (a < b) {
    return -1;
  } else if (a > b) {
    return 1;
  }
  return 0;
}

void*
memory_address_duplicate(void const *address)
{
  return (void*) address;
}

void
memory_address_free(void *address)
{
  /* no-op */
}
