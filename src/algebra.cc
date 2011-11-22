
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

void
array_matrix_multiply(int const rowsA, int const colsB, int const colsA, 
		      double const *A, int const lba, double const *B, int const lbb,
		      double *C, int const lbc)
{
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 
	      rowsA, colsB, colsA, 1.0, A, lba, B, lbb, 0.0, C, lbc);
}

