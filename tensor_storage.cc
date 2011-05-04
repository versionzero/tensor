/*----------------------------------------------------------------------
  Headers
  ----------------------------------------------------------------------*/

#include "error.h"
#include "storage.h"

/*----------------------------------------------------------------------  
  Main Functions
----------------------------------------------------------------------*/

void
validate_strategy(int code) {
  if (0 > code || code > storage::strategy::max) {
    error(E_STRATEGY, code);
  }
}

/*--------------------------------------------------------------------*/

storage::strategy::type
extract_stategy(int code)
{                               /* --- convert an integer opcode to a
				   valid operation code */
  validate_strategy(code);
  return static_cast<storage::strategy::type>(code);
}

/*--------------------------------------------------------------------*/

char const*
strategy_name(int code)
{
  static char const *names[] = {
    "Canonical",
    "Naive Extended Karnaugh Map",
    "Row-Major Order Extended Karnaugh Map",
    "Compressed Row Storage Extended Karnaugh Map"
  };
  validate_strategy(code);
  return names[code];
}

/*----------------------------------------------------------------------
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
