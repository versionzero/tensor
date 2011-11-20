
#include "algebra.h"
#include "error.h"
#include "cblas.h"

#if 0
#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#endif
#ifdef __linux__
#include <cblas.h>
#endif
#endif

double
array_inner_product(const int N, const double *X, const int incX, const double *Y, const int incY)
{
  return cblas_ddot(N, X, incX, Y, incY);
}
