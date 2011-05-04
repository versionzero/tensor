/*----------------------------------------------------------------------
  File    : random.c
  Contents: random number generators
  Author  : Christian Borgelt
  History : 2008.10.05 file created
----------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "random.h"

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
RANDOM *rng_curr = NULL;        /* current random number generator */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/

RANDOM* rng_create (int type, unsigned int *seeds)
{                               /* --- create random number generator */
  int    i;                     /* loop variable */
  RANDOM *rng;                  /* created random number generator */

  rng = (RANDOM*)malloc(sizeof(RANDOM) +5 *sizeof(unsigned int));
  if (!rng) return NULL;        /* create a random number generator */
  if (seeds) for (i = 5; --i >= 0; ) rng->regs[i] = seeds[i];
  else       for (i = 5; --i >= 0; ) rng->regs[i] = rand();
  return rng;                   /* initialize the generator */
}  /* rng_create() */           /* and return it */

/*--------------------------------------------------------------------*/

unsigned int rng_uint (RANDOM *rng)
{                               /* --- generate a random integer */
  unsigned int t = (rng->regs[0] ^ (rng->regs[0] >> 7));
  rng->regs[0] = rng->regs[1];
  rng->regs[1] = rng->regs[2];
  rng->regs[2] = rng->regs[3];
  rng->regs[3] = rng->regs[4];
  rng->regs[4] = (rng->regs[4] ^ (rng->regs[4] << 6)) ^ (t ^ (t << 13));
  return (rng->regs[1] +rng->regs[1] +1) *rng->regs[4];
}  /* rng_uint() */

/*--------------------------------------------------------------------*/

int rng_int (RANDOM *rng)
{                               /* --- generate a random integer */
  int t = (int)rng_uint(rng);   
  return (t < 0) ? t -INT_MIN : t;
}  /* rng_int() */
