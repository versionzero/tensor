/*----------------------------------------------------------------------
  Headers
  ----------------------------------------------------------------------*/

#include "error.h"
#include "operation.h"

/*----------------------------------------------------------------------  
  Main Functions
----------------------------------------------------------------------*/

void
validate_operation(int code) {
  if (0 > code || code > operations::arithmetic::max) {
    error(E_OPTCODE, code);
  }
}

/*--------------------------------------------------------------------*/

operations::arithmetic::type
extract_operation(int code)
{                               /* --- convert an integer opcode to a
				   valid operation code */
  validate_operation(code);
  return static_cast<operations::arithmetic::type>(code);
}

/*--------------------------------------------------------------------*/

char const*
operation_name(int code)
{
  static char const *names[] = {
    "addition",
    "subtraction",
    "multiplication"
  };
  validate_operation(code);
  return names[code];
}

/*----------------------------------------------------------------------
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
