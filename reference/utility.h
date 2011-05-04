
#pragma once
#ifndef _UTILITY_H_
#define _UTILITY_H_

/*--------------------------------------------------------------------*/

/* The following macro allows us to dispence with the indentation
   requirements associated with starting a new scope, but nevertheless
   allows us to use namespaces as we should all be doing. */
#define BEGIN_NAMESPACE(x) namespace x {
#define   END_NAMESPACE(x) }

/*--------------------------------------------------------------------*/

/* Very simple calculation of elapsed time */
#define SEC_SINCE(t)  ((clock()-(t))/(double)CLOCKS_PER_SEC)

/*--------------------------------------------------------------------*/
 
#endif /* _UTILITY_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
