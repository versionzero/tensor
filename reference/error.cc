/*----------------------------------------------------------------------
  Headers
  ----------------------------------------------------------------------*/

#include "error.h"

#include <cassert>
#include <cstdarg>
#include <iostream>

/*----------------------------------------------------------------------
  Namespace inclusions
----------------------------------------------------------------------*/

using std::cerr;
using std::endl;

/*----------------------------------------------------------------------  
  Global Variables
----------------------------------------------------------------------*/

extern char *name;              /* program name for error messages */

/*----------------------------------------------------------------------
  Constants
----------------------------------------------------------------------*/

/* --- error messages --- */
static const char *errors[] = {
  /* E_NONE       0 */  "no error",
  /* E_NOMEM     -1 */  "not enough memory",
  /* E_FOPEN     -2 */  "cannot open file %s",
  /* E_FREAD     -3 */  "read error on file %s",
  /* E_FWRITE    -4 */  "write error on file %s",
  /* E_OPTION    -5 */  "unknown option -%c",
  /* E_OPTARG    -6 */  "missing option argument",
  /* E_ARGCNT    -7 */  "wrong number of arguments",
  /* E_TNSDIMS   -8 */  "tensor has incorrect dimensions (multiple of 4)",
  /* E_BLOCKSZ   -9 */  "cache block has incorrect size (power of 2)",
  /* E_STRATEGY -10 */  "unknown storage strategy %d",
  /* E_OPTCODE  -11 */  "unknown arithmetic operation %d",
  /* E_UNKNOWN  -12 */  "unknown error"
};

/*----------------------------------------------------------------------  
  Main Functions
----------------------------------------------------------------------*/

void 
error(int code, ...)
{                               /* --- print an error message */
  va_list    args;              /* list of variable arguments */
  const char *msg;              /* error message */
  char       buffer[1024];

  if (code < E_UNKNOWN) {
    code = E_UNKNOWN;
  }
  if (code < 0) {               /* if to report an error, */
    msg = errors[-code];        /* get the error message */
    if (!msg) {
      msg = errors[-E_UNKNOWN];
    }
    cerr << endl << name << ": error: ";
    va_start(args, code);       /* get variable arguments */
    vsprintf(buffer, msg, args);/* format error message */
    va_end(args);               /* end argument evaluation */
    cerr << buffer << endl;     /* print error message */
  }
  exit(code);                   /* abort the program */
}  /* error() */

/*----------------------------------------------------------------------
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
