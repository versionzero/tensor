
#include "error.h"
#include "matrix.h"
#include "mmio.h"

#include <stdio.h>
#include <stdlib.h>

uint worker_threshold;

/*
  1) 2-by-2 block matrix:
  
  | C_11 C_12 | = | A_11 A_21 | * | B_11 B_12 |
  | C_21 C_22 |   | A_21 A_22 |   | B_21 B_22 |
  
  2) Strassen (1969) has shown how to compute C with just 7
  multiplies and 18 adds:
  
  P_1  = (A_11 + A_22) * (B_11 + B_22);
  P_2  = (A_21 + A_22) * (B_11);
  P_3  = (A_11)        * (B_12 - B_22);
  P_4  = (A_22)        * (B_21 - B_11);
  P_5  = (A_11 + A_12) * (B_22);
  P_6  = (A_21 - A_11) * (B_11 + B_12);
  P_7  = (A_12 - A_22) * (B_21 + B_22);
  C_11 = P_1 + P_4 - P_5 + P_7;
  C_12 = P_3 + P_5;
  C_21 = P_2 + P_4;
  C_22 = P_1 + P_3 - P_2 + P_6;
  
  Algorithm 1.3.1 (Strassen Multiplication), "Matrix Computations,"
  G. H. Golub,  pp. 31-32.
  
  function: C = strass(A,B,n,n_{min})
    if n \le n_{min}
      C = AB
    else
      m = n/2; 
      u = 1:m; 
      v = m+1:n;
      P_1  = strass((A(u,u) + A(v,v)),(B(u,u) + B(v,v)),m,n_{min})
      P_2  = strass((A(v,u) + A(v,v)),(B(u,u)),         m,n_{min})
      P_3  = strass((A(u,u)),         (B(u,v) - B(v,v)),m,n_{min})
      P_4  = strass((A(v,v)),         (B(v,u) - B(u,u)),m,n_{min})
      P_5  = strass((A(u,u) + A(u,v)),(B(v,v)),         m,n_{min})
      P_6  = strass((A(v,u) - A(u,u)),(B(u,u) + B(u,v)),m,n_{min})
      P_7  = strass((A(u,v) - A(v,v)),(B(v,u) + B(v,v)),m,n_{min})
      C(u,u) = P_1 + P_4 - P_5 + P_7
      C(u,v) = P_3 + P_5
      C(v,u) = P_2 + P_4
      C(v,v) = P_1 + P_3 - P_2 + P_6
    end
      
*/

#define NUM_BLOCKS 8

matrix_t*
matrix_multiply_strassen(matrix_t const *m1, matrix_t const *m2, uint n)
{
  uint     i, j;
  uint     m, u, v;
  matrix_t *c, *a[3][3], *b[3][3], *p[NUM_BLOCKS], *t1, *t2;
  
  if (NULL == (c = matrix_new(m1->m, m1->n))) {
    error(D_ERROR|D_FATAL, "Failed to allocate output matrix.\n");
  }
  
  if (n <= worker_threshold) {
    
    matrix_multiply_inplace(c, m1, m2);
    
  } else {
    
    m = n/2;
    u = 0;
    v = m;
    
    a[1][1] = matrix_partition(m1, u, u, m, m);
    a[1][2] = matrix_partition(m1, u, v, m, n);
    a[2][1] = matrix_partition(m1, v, u, n, m);
    a[2][2] = matrix_partition(m1, v, v, n, n);
    
    b[1][1] = matrix_partition(m2, u, u, m, m);
    b[1][2] = matrix_partition(m2, u, v, m, n);
    b[2][1] = matrix_partition(m2, v, u, n, m);
    b[2][2] = matrix_partition(m2, v, v, n, n);
    
    t1      = matrix_new(m, m);
    t2      = matrix_new(m, m);
    
    /* P_1  = (A_11 + A_22) * (B_11 + B_22); */
    matrix_copy_inplace(t1, a[1][1]);
    matrix_add_inplace (t1, a[2][2]);
    matrix_copy_inplace(t2, b[1][1]);
    matrix_add_inplace (t2, b[2][2]);
    p[1] = matrix_multiply_strassen(t1, t2, m); 
    
    /* P_2  = (A_21 + A_22) * (B_11); */
    matrix_copy_inplace(t1, a[2][1]);
    matrix_add_inplace (t1, a[2][2]);
    p[2] = matrix_multiply_strassen(t1, b[1][1], m); 

    /* P_3  = (A_11)        * (B_12 - B_22); */
    matrix_copy_inplace    (t2, b[1][2]);
    matrix_subtract_inplace(t2, b[2][2]);
    p[3] = matrix_multiply_strassen(a[1][1], t2, m);
    
    /* P_4  = (A_22)        * (B_21 - B_11); */
    matrix_copy_inplace    (t2, b[2][1]);
    matrix_subtract_inplace(t2, b[1][1]);
    p[4] = matrix_multiply_strassen(a[2][2], t2, m);
    
    /* P_5  = (A_11 + A_12) * (B_22); */
    matrix_copy_inplace(t1, a[1][1]);
    matrix_add_inplace (t1, a[1][2]);
    p[5] = matrix_multiply_strassen(t1, b[2][2], m); 
    
    /* P_6  = (A_21 - A_11) * (B_11 + B_12); */
    matrix_copy_inplace    (t1, a[2][1]);
    matrix_subtract_inplace(t1, a[1][1]);
    matrix_copy_inplace    (t2, b[1][1]);
    matrix_add_inplace     (t2, b[1][2]);
    p[6] = matrix_multiply_strassen(t1, t2, m); 
    
    /* P_7  = (A_12 - A_22) * (B_21 + B_22); */
    matrix_copy_inplace    (t1, a[1][2]);
    matrix_subtract_inplace(t1, a[2][2]);
    matrix_copy_inplace    (t2, b[2][1]);
    matrix_add_inplace     (t2, b[2][2]);
    p[7] = matrix_multiply_strassen(t1, t2, m); 
    
    /* C_11 = P_1 + P_4 - P_5 + P_7; */
    matrix_copy_inplace    (a[1][1], p[1]);
    matrix_add_inplace     (a[1][1], p[4]);
    matrix_subtract_inplace(a[1][1], p[5]);
    matrix_add_inplace     (a[1][1], p[7]);
    
    /* C_12 = P_3 + P_5; */
    matrix_copy_inplace(a[1][2], p[3]);
    matrix_add_inplace (a[1][2], p[5]);
    
    /* C_21 = P_2 + P_4; */
    matrix_copy_inplace(a[2][1], p[2]);
    matrix_add_inplace (a[2][1], p[4]);
    
    /* C_22 = P_1 + P_3 - P_2 + P_6; */
    matrix_copy_inplace    (a[2][2], p[1]);
    matrix_add_inplace     (a[2][2], p[3]);
    matrix_subtract_inplace(a[2][2], p[2]);
    matrix_add_inplace     (a[2][2], p[6]);
    
    /* for all i, j, merge c[i][j] into c */
    matrix_copy_inplace_with_offset(c, 0, 0, a[1][1]);
    matrix_copy_inplace_with_offset(c, 0, m, a[1][2]);
    matrix_copy_inplace_with_offset(c, m, 0, a[2][1]);
    matrix_copy_inplace_with_offset(c, m, m, a[2][2]);
    
    matrix_delete(t1);
    matrix_delete(t2);
    for (i = 1; i < NUM_BLOCKS; ++i) {
      matrix_delete(p[i]);
    }
    for (i = 1; i <= 2; ++i) {
      for (j = 1; j <= 2; ++j) {
	matrix_delete(a[i][j]);
	matrix_delete(b[i][j]);
      }
    }
    
  }
  
  return c;
}

matrix_t*
matrix_multiply_strassen(matrix_t const *m1, matrix_t const *m2)
{
  matrix_supported(m1);
  matrix_compatible(m1, m2);  
  return matrix_multiply_strassen(m1, m2, m1->n);
}
