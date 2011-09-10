
#include "random.h"

static uint w = 1;
static uint z = 2;

void
random_seed(uint seed)
{
  if (seed > 0) {
    w = seed + 1;
    z = w + w + 1;
  }
}

/* http://en.wikipedia.org/wiki/Random_number_generator */
uint
random_marsaglia()
{
  z = 36969 * (z & 65535) + (z >> 16);
  w = 18000 * (w & 65535) + (w >> 16);
  return (z << 16) + w;  /* 32-bit result */
}

/* generate a random number in the range [min, max] */
uint 
random_between(uint min, uint max)
{
  return random_marsaglia() % (max-min+1) + min;
}
