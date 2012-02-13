
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int w = 1;
static int z = 2;

//#define debugf printf
#define debugf 

void
random_seed(int seed)
{
  if (seed > 0) {
    w = seed + 1;
    z = w + w + 1;
  }
}

/* http://en.wikipedia.org/wiki/Random_number_generator */
int
random_marsaglia()
{
  z = 36969 * (z & 65535) + (z >> 16);
  w = 18000 * (w & 65535) + (w >> 16);
  return (z << 16) + w;  /* 32-bit result */
}

int 
random_between(int min, int max)
{
  return random_marsaglia() % (max-min+1) + min;
}

void
vector_fill(double *A, int n, double d = 0.0)
{
  int i;
  
  for (i = 0; i < n; i++) {
    A[i] = d;
  }
}

void
matrix_fill(double *A, int n)
{
  int i, j;
  
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      A[i * n + j] = 0.0;
    }
  }
}

void
tensor_fill(double *A, int n)
{
  int i, j, k;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
	A[k * n * n + i * n + j] = 0.0;
      }
    }
  }
}

void
tensor_init(double *A, int n, int m)
{
  int i, j, k;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
	if (random_between(0, m) == 0) {
	  A[k * n * n + i * n + j] = 1.0;
	}
      }
    }
  }
}

void
vector_print(double *A, int n)
{
  int i;
  
  for (i = 0; i < n; i++) {
    printf("%g ", A[i]);
  }
  printf("\n");
}

void
matrix_print(double *A, int n)
{
  int i, j;
  
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      printf("%g ", A[i * n + j]);
    }
    printf("\n");
  }
}

void
tensor_print(double *A, double *S, int n)
{
  int i, j, k;
  
  for (k = 0; k < n; k++) {
    if (S[k] != 0.0) {
      for (i = 0; i < n; i++) {
	for (j = 0; j < n; j++) {
	  printf("%g ", A[k * n * n + i * n + j]);
	}
	printf("\n");
      }
      printf("\n");
    }
  }
}

void
tensor_print_diag(double *A, int n)
{
  int i, j, k;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      printf("%2d  ", k * n * n + i * n);
      for (j = 0; j < i; j++) {	
	printf("  ");
      }
      for (j = i; j < n + i; j++) {
	printf("%g ", A[k * n * n + i * n + (j % n)]);
      }
      printf("\n");
    }
    break;
    printf("\n");
  }
}

int
diag_to_row_col(int i, int j, int n) 
{
  int d;
  
  d = i + j * n + j;
  if (i + j >= n) {
    d -= n;
  }
  
  return d;
}

void
compress_diagonal(double *A, int n)
{
  int  i, j, k, m, cur, prev;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      debugf("i=%d\n", i);
      j = m = cur = prev = 0;
      while (j < n) {
	/* find first zero diagonal entry */
	for (j = m; j < n; j++) {
	  cur = k * n * n + diag_to_row_col(i, j, n);
	  if (A[cur] == 0.0) {
	    debugf("done> %g %d\n", A[cur], cur);
	    prev = cur;
	    m    = j;
	    break;
	  }
	}
	/* move the next non-zero such that it is contiguous with
	   those previously seen */
	for (; j < n; j++) {
	  cur = k * n * n + diag_to_row_col(i, j, n);	
	  if (A[cur] != 0.0) {
	    debugf(" done> %g %d -> %d\n", A[cur], cur, prev);
	    A[prev] = A[cur];
	    A[cur]  = 0.0;
	    break;
	  } else {
	    debugf("      cur> %g %d\n", A[cur], cur);
	  }
	}
	debugf("\n\nj = %d; n = %d\n\n", j, n);
      }
      debugf("\n");
      //print_diag(A, n);
      //debugf("\n");
    }
  }
}

void
compress_column(double *A, int n)
{
  int  i, j, k, m, cur, prev;
  
  for (k = 0; k < n; k++) {
    for (j = 0; j < n; j++) {
      i = m = cur = prev = 0;
      while (i < n) {
	/* find first zero diagonal entry */
	for (i = m; i < n; i++) {
	  cur = k * n * n + i * n + j;
	  if (A[cur] == 0.0) {
	    prev = cur;
	    m    = i;
	    break;
	  }
	}
	/* move the next non-zero such that it is contiguous with
	   those previously seen */	
	for (; i < n; i++) {
	  cur = k * n * n + i * n + j;
	  if (A[cur] != 0.0) {
	    A[prev] = A[cur];
	    A[cur]  = 0.0;
	    break;
	  } 
	}
      }
    }
  }
}

void
compress_row(double *A, int n)
{
  int  i, j, k, m, cur, prev;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      j = m = cur = prev = 0;
      while (j < n) {
	/* find first zero row entry */
	for (j = m; j < n; j++) {
	  cur = k * n * n + i * n + j;
	  if (A[cur] == 0.0) {
	    prev = cur;
	    m    = j;
	    break;
	  }
	}
	/* move the next non-zero such that it is contiguous with
	   those previously seen */	
	for (; j < n; j++) {
	  cur = k * n * n + i * n + j;
	  if (A[cur] != 0.0) {
	    A[prev] = A[cur];
	    A[cur]  = 0.0;
	    break;
	  } 
	}
      }
    }
  }
}

int
count_tubes(double *A, double *B, int n, int m)
{
  int  i, j, k, t, u, cur;
  
  t = 0;
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      u = 0;
      for (k = 0; k < n; k++) {
	cur = k * n * n + i * n + j;
	if (A[cur] != 0.0) {
	  u++;
	}
      }
      if (u == m) {
	B[i * n + j] = 1.0;
	t++;
      }
    }
  }
  
  return t;
}

void
drop_tubes(double *A, double *B, int n)
{
  int  i, j, k, m, cur;
  
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      if (B[i * n + j] == 1.0) {
	for (k = 0; k < n; k++) {
	  cur = k * n * n + i * n + j;
	  A[cur] = 0.0;
	}
      }
    }
  }
}

int
mark_empty_slices(double *A, double *S, int n)
{
  int i, j, k, m, cur;
  bool empty;
  
  m = 0;
  for (k = 0; k < n; k++) {
    if (S[k] != 0.0) {
      empty = true;
      for (i = 0; i < n && empty; i++) {
	for (j = 0; j < n && empty; j++) {
	  cur = k * n * n + i * n + j;
	  if (A[cur] != 0.0) {
	    empty = false;
	  }
	}
      }
      if (empty) {
	S[k] = 0.0;
	m++;
      }
    }
  }
  
  return m;
}

int
main(int argc, char *argv[])
{
  int m, n, r, vectors, seed, density;
  double *A, *B, *S;
  
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <n> [seed] [density (1 in n)]\n", argv[0]);
    exit(1);
  }
  
  n       = atoi(argv[1]);
  seed    = argc > 2 ? atoi(argv[2]) : 0;
  density = argc > 3 ? atoi(argv[3]) : 10;
  A       = (double*) malloc(n * n * n * sizeof(double));
  B       = (double*) malloc(n * n * sizeof(double));
  S       = (double*) malloc(n * sizeof(double));
  
  vector_fill(S, n, 1.0);
  tensor_fill(A, n);
  
  random_seed(seed);
  tensor_init(A, n, density);
  
  r = vectors = 0;
  while (r < n) {
    
    m = mark_empty_slices(A, S, n);
  
    //printf("Removed %d empty matricies:\n\n", m);
    //vector_print(S, n);
    //printf("\n");
    
    r += m;
    if (r == n) {
      break;
    }
    
    vectors++;
    
    //printf("A^[%d] (seed=%d):\n\n", n, seed);
    //tensor_print(A, S, n);
    //printf("\ndiag(A^[%d]) (seed=%d):\n\n", n, seed);
    //tensor_print_diag(A, n);
    //printf("\n");
    
    compress_diagonal(A, n);
    
    //printf("compress_diagonal(diag(A^[%d])) (seed=%d):\n\n", n, seed);
    //tensor_print_diag(A, n);
    //printf("\ncompress_diagonal(A^[%d]):\n\n", n, seed);
    //tensor_print(A, S, n);
    //printf("\n");
    
    compress_column(A, n);
    
    //printf("\ncompress_column(A^[%d]):\n\n", n, seed);
    //tensor_print(A, S, n);
    //printf("\n");
    
    compress_row(A, n);
    
    //printf("\ncompress_row(A^[%d]):\n\n", n, seed);
    //tensor_print(A, S, n);
    //printf("\n");
    
    matrix_fill(B, n);
    m = count_tubes(A, B, n, n - r);
    
    printf("%d size %d\n", m, n - r);
    //printf("There are %d dense tubes of size %d:\n\n", m, n - r);
    //matrix_print(B, n);
    //printf("\n");
    
    drop_tubes(A, B, n);
    
    //printf("\nA^[%d]:\n\n", n, seed);
    //tensor_print(A, S, n);
    //printf("\n");
    
  }
  
  printf("Requires %d sub-vectors\n", vectors);
  
  free(S);
  free(B);
  free(A);
  
  return 0;
}
