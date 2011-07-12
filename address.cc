
#include "address.h"

/* Source: http://www.concentric.net/~ttwang/tech/addrhash.htm
   
   Fibonacci hash function.  The multiplier is the nearest prime to
   (2^32 times (√5 - 1)/2). (2654435761 is the golden ratio of 2^32.)
   See Knuth §6.4: volume 3, 3rd ed, p518 */
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
