/*----------------------------------------------------------------------
  File    : random.h
  Contents: random number generators
  Author  : Christian Borgelt
  History : 2008.10.05 file created
----------------------------------------------------------------------*/
#ifndef __RANDOM__
#define __RANDOM__
#include <limits.h>

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define RNG_XORSHIFT    0       /* generator based on xor and shift */

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/
typedef struct {                /* --- random number generator --- */
  int          type;            /* generator type */
  unsigned int regs[1];         /* registers to store the state */
} RANDOM;                       /* (random number generator) */

/*----------------------------------------------------------------------
  Global Variables
----------------------------------------------------------------------*/
extern RANDOM* rng_curr;        /* current random number generator */

/*----------------------------------------------------------------------
  Functions
----------------------------------------------------------------------*/
extern RANDOM*      rng_create (int type, unsigned int *seeds);
extern unsigned int rng_uint   (RANDOM *rng);
extern int          rng_int    (RANDOM *rng);
extern double       rng_double (RANDOM *rng);

extern RANDOM*      rng_get    (void);
extern void         rng_set    (RANDOM *rng);
extern int          rng_rand   (void);

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/
#define rng_double(r)  (rng_unit(r) /((double)UINT_MAX +1.0));

#define rng_get()      (rng_curr)
#define rng_set(r)     (rng_curr = (r))
#define rng_rand(r)    rng_int(rng_curr)

#endif
