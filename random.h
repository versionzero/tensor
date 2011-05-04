
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include "types.h"

void random_seed(uint seed);
uint random_marsaglia();
uint random_between(uint min, uint max);

#endif

