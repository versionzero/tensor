
#include "algebra.h"
#include "error.h"

#ifdef __APPLE__
#include <Accelerate/Accelerate.h>
#else
#include <cblas.h>
#endif

double
array_inner_product(const int N, const double *X, const int incX, const double *Y, const int incY)
{
  return cblas_ddot(N, X, incX, Y, incY);
}
