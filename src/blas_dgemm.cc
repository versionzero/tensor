
/*  -- translated by f2c (version 19940927).
 You must link the resulting object file with the libraries:
 -lf2c -lm   (in that order)

 From: http://trac.mcs.anl.gov/projects/performance/browser/pbound/examples/clapack/clapack/cblas/dgemm.c?rev=515
 */

//#include "f2c.h"
//#include "dgemm.h"
#include <stdio.h>
#include <math.h>
#include "cblas.h"
#include "error.h"

#define A(I,J) a[(I)-1 + ((J)-1)* ( *lda)]
#define B(I,J) b[(I)-1 + ((J)-1)* ( *ldb)]
#define C(I,J) c[(I)-1 + ((J)-1)* ( *ldc)]


#if 0
void usage()
{
  printf(" \n USAGE: -h - help \n -i input_file \n -o ouput_file \n -a a_file \n -x x_file \n"); 
  exit(0);
}
#endif

long int lsame_(char const *a, char const *b)
{
  if(a ==b)
   return 1;
  else
   return 0;
}

long int max(long int a, long int b)
{
  if(a < b)
   return b;
  else
   return a;
}

void dgemm_(char *transa, char *transb, int const *m, int const *n, int const *k,
	    double const *alpha, double const *a, int const *lda, double const *b, 
	    int const *ldb, double const *beta, double *c, int const *ldc);

void 
cblas_dgemm(const enum CBLAS_ORDER Order, const enum CBLAS_TRANSPOSE TransA,
	    const enum CBLAS_TRANSPOSE TransB, const int M, const int N,
	    const int K, const double alpha, const double  *A,
	    const int lda, const double  *B, const int ldb,
	    const double beta, double  *C, const int ldc)
{
  char TA, TB;
  
  if (Order == CblasColMajor) {
    if (TransA == CblasTrans)            TA='T';
    else if ( TransA == CblasConjTrans ) TA='C';
    else if ( TransA == CblasNoTrans )   TA='N';
    else die("cblas_dgemm: Illegal TransA setting, %d\n", TransA);
    if (TransB == CblasTrans)            TB='T';
    else if ( TransB == CblasConjTrans ) TB='C';
    else if ( TransB == CblasNoTrans )   TB='N';
    else die("cblas_dgemm: Illegal TransB setting, %d\n", TransB);
  } else if (Order == CblasRowMajor) {
    if(TransA == CblasTrans)             TB='T';
    else if ( TransA == CblasConjTrans ) TB='C';
    else if ( TransA == CblasNoTrans )   TB='N';
    else die("cblas_dgemm: Illegal TransA setting, %d\n", TransA);
    if(TransB == CblasTrans) TA='T';
    else if ( TransB == CblasConjTrans ) TA='C';
    else if ( TransB == CblasNoTrans )   TA='N';
    else die("cblas_dgemm: Illegal TransB setting, %d\n", TransB);
  } else {
    die("cblas_dgemm: Illegal Order setting, %d\n", Order);
  }
  
  dgemm_(&TA, &TB, &M, &N, &K, &alpha, B, &ldb, A, &lda, &beta, C, &ldc);
}

/* Subroutine */ 
void 
dgemm_(char *transa, char *transb, int const *m, int const *n, int const *k,
       double const *alpha, double const *a, int const *lda, double const *b, 
       int const *ldb, double const *beta, double *c, int const *ldc)
{
  
  
  /* System generated locals */
  //long int a_dim1, a_offset, b_dim1, b_offset, c_dim1, c_offset;
  long int i__1, i__2, i__3;
  
  /* Local variables */
  static long int info;
  static long int nota, notb;
  static double temp;
  static long int i, j, l, ncola;
  //extern long int lsame_(char *, char *);
  static long int nrowa, nrowb;
  extern /* Subroutine */ int xerbla_(char *, long int *);
   
  
  nota = lsame_(transa, "N");
  notb = lsame_(transb, "N");
  if (nota) {
    nrowa = *m;
    ncola = *k;
  } else {
    nrowa = *k;
    ncola = *m;
  }
  if (notb) {
    nrowb = *k;
  } else {
    nrowb = *n;
  }
  
  /*     Test the input parameters. */
  
  info = 0;
  if (! nota && ! lsame_(transa, "C") && ! lsame_(transa, "T")) {
    info = 1;
  } else if (! notb && ! lsame_(transb, "C") && ! lsame_(transb, 
                                                         "T")) {
    info = 2;
  } else if (*m < 0) {
    info = 3;
  } else if (*n < 0) {
    info = 4;
  } else if (*k < 0) {
    info = 5;
  } else if (*lda < max(1,nrowa)) {
    info = 8;
  } else if (*ldb < max(1,nrowb)) {
    info = 10;
  } else if (*ldc < max(1,*m)) {
    info = 13;
  }
 /* if (info != 0) {
    xerbla_("DGEMM ", &info);
    return 0;
  }*/
  
  /*     Quick return if possible. */
  
  if (*m == 0 || *n == 0 || ((*alpha == 0. || *k == 0) && *beta == 1.)) {
    return;
  }
  
  /*     And if  alpha.eq.zero. */
  
  if (*alpha == 0.) {
    if (*beta == 0.) {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
        i__2 = *m;
        for (i = 1; i <= *m; ++i) {
          C(i,j) = 0.;
          /* L10: */
        }
        /* L20: */
	    }
    } else {
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
        i__2 = *m;
        for (i = 1; i <= *m; ++i) {
          C(i,j) = *beta * C(i,j);
          /* L30: */
        }
        /* L40: */
	    }
    }
    return;
  }
 /*   dgemm(nota, notb, alpha, beta, m, n, k, a, b, c, lda, ldb, ldc)
  return 0; 
}  

void dgemm(nota, notb, alpha, beta, m, n, k, a, b, c, lda, ldb, ldc)
{  
*/    
  /*     Start the operations. */
  
  if (notb) {
    if (nota) {
      
      /*           Form  C := alpha*A*B + beta*C. */
      
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
        if (*beta == 0.) {
          i__2 = *m;
          for (i = 1; i <= *m; ++i) {
            C(i,j) = 0.;
            /* L50: */
          }
        } else if (*beta != 1.) {
          i__2 = *m;
          for (i = 1; i <= *m; ++i) {
            C(i,j) = *beta * C(i,j);
            /* L60: */
          }
        }
        i__2 = *k;
        for (l = 1; l <= *k; ++l) {
          if (B(l,j) != 0.) {
            temp = *alpha * B(l,j);
            i__3 = *m;
            for (i = 1; i <= *m; ++i) {
              C(i,j) += temp * A(i,l);
              /* L70: */
            }
          }
          /* L80: */
        }
        /* L90: */
	    }
    } else {
      
      /*           Form  C := alpha*A'*B + beta*C */
      
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
        i__2 = *m;
        for (i = 1; i <= *m; ++i) {
          temp = 0.;
          i__3 = *k;
          for (l = 1; l <= *k; ++l) {
            temp += A(l,i) * B(l,j);
            /* L100: */
          }
          if (*beta == 0.) {
            C(i,j) = *alpha * temp;
          } else {
            C(i,j) = *alpha * temp + *beta * C(i,j);
          }
          /* L110: */
        }
        /* L120: */
	    }
    }
  } else {
    if (nota) {
      
      /*           Form  C := alpha*A*B' + beta*C */
      
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
        if (*beta == 0.) {
          i__2 = *m;
          for (i = 1; i <= *m; ++i) {
            C(i,j) = 0.;
            /* L130: */
          }
        } else if (*beta != 1.) {
          i__2 = *m;
          for (i = 1; i <= *m; ++i) {
            C(i,j) = *beta * C(i,j);
            /* L140: */
          }
        }
        i__2 = *k;
        for (l = 1; l <= *k; ++l) {
          if (B(j,l) != 0.) {
            temp = *alpha * B(j,l);
            i__3 = *m;
            for (i = 1; i <= *m; ++i) {
              C(i,j) += temp * A(i,l);
              /* L150: */
            }
          }
          /* L160: */
        }
        /* L170: */
	    }
    } else {
      
      /*           Form  C := alpha*A'*B' + beta*C */
      
	    i__1 = *n;
	    for (j = 1; j <= *n; ++j) {
        i__2 = *m;
        for (i = 1; i <= *m; ++i) {
          temp = 0.;
          i__3 = *k;
          for (l = 1; l <= *k; ++l) {
            temp += A(l,i) * B(j,l);
            /* L180: */
          }
          if (*beta == 0.) {
            C(i,j) = *alpha * temp;
          } else {
            C(i,j) = *alpha * temp + *beta * C(i,j);
          }
          /* L190: */
        }
        /* L200: */
	    }
    }
  }
  

  
  /*     End of DGEMM . */
  
} /* dgemm_ */

#if 0

int main(int argc, char **argv)
{
  int iflag = 0,  xflag = 0, aflag = 0,  oflag = 0, usage_flag = 0;
  FILE * input_file, *output_file, *x_file, *a_file;
  char ch;
  
  int  i, n;
  double a1, a2, a3, a4, *x1 , *x2 , *x3, *x4, *y;

  while ((ch = getopt(argc, argv, "i:o:h")) != -1) {
    switch (ch) {
      case 'h':
        usage();
        break;
      case 'i':
        iflag++;
        if ((input_file = fopen(optarg, "r")) < 0) {
          (void)fprintf(stderr,
                        "Unable to open : %s \n", optarg);
          usage_flag=1;
        }
        break; 
      case 'o':
        oflag++;
        if ((output_file = fopen(optarg, "w")) < 0) {
          (void)fprintf(stderr,
                        "Unable to create : %s \n", optarg);
          usage_flag=1;
        }
        break;
      /*case 'r':
        if (optarg<=0) {
          (void)fprintf(stderr,
                        " Repeat should be greater than zero \n");
          usage_flag=1;
        }
        repeat = strtoimax(optarg,NULL,10);    
        break;5
      case 's':
        if (optarg<=0) {
          (void)fprintf(stderr,
                        " Iter should be greater than zero \n");
          iter=1;
        }
        iter = strtoimax(optarg,NULL,10);    
        break; */       
      case '?':
      default:
        usage();
    }
  }
  
  if(iflag==0 || oflag==0 || usage_flag==1){
    usage();
  }
  



  
  x1 = (double *) malloc(n * sizeof(double));
  x2 = (double *) malloc(n * sizeof(double));
  x3 = (double *) malloc(n * sizeof(double));
  x4 = (double *) malloc(n * sizeof(double));
  y =  (double *) malloc(n * sizeof(double));
    
 
  //papi_axpy4(output_file, n, y, a1, x1, a2, x2, a3, x3, a4, x4);
  
  return 0;


}

#endif
