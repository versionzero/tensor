
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

#if 0
ml_matrix matrix_multiply(const ml_matrix &A,const ml_matrix &B)
{
    ml_matrix C(A.rows(),B.cols());
    cblas_dgemm(ML_BLAS_STORAGE,CblasNoTrans,CBlasNoTrans,
                A.rows(),B.cols(),A.rows(),
                1.0,A.raw_data(),A.data_stride(),
                B.raw_data(),B.data_stride(),
                0.0,C.raw_data(),C.data_stride());
    return C;
}
#endif
