
#ifndef _ARRAY_MATH_H_
#define _ARRAY_MATH_H_

double array_inner_product(int const N, double const *X, int const incX, double const *Y, const int incY);
void array_matrix_multiply(int const M, int const N, int const K, 
			   double const *A, int const lba, 
			   double const *B, int const lbb, 
			   double *C, int const lbc);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
