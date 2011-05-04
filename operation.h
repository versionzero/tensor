
#pragma once
#ifndef _OPERATION_H_
#define _OPERATION_H_

/*--------------------------------------------------------------------*/

#include "utility.h"

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(operations);

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(arithmetic);

typedef enum {
  addition       = 0,
  subtraction    = 1,
  multiplication = 2,
  max            = multiplication
} type;

END_NAMESPACE(arithmetic);

/*--------------------------------------------------------------------*/

END_NAMESPACE(operations);

/*--------------------------------------------------------------------*/

void validate_operation(int code);
operations::arithmetic::type extract_operation(int code);
char const* operation_name(int code);

/*--------------------------------------------------------------------*/

#endif /* _OPERATION_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
