
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static int w = 1;
static int z = 2;

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
zero(double *A, int n)
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
init(double *A, int n, int m)
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
print(double *A, int n)
{
  int i, j, k;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      for (j = 0; j < n; j++) {
	printf("%g ", A[k * n * n + i * n + j]);
      }
      printf("\n");
    }
    break;
    printf("\n");
  }
}

void
print_diag(double *A, int n)
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
diag_to_row_col(int i, int j, int n) {
  int d;
  
  d = i + j * n + j;
  if (i + j >= n) {
    d -= n;
  }
  
  return d;
}

void
compress_diag(double *A, int n)
{
  int  i, j, k, m, cur, prev;
  bool searching, swapping;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      printf("i=%d\n", i);
      j = cur = prev = 0;
      while (j < n) {
	/* find first zero diagonal entry */
	searching = true;
	for (j = prev; j < n && searching; j++) {
	  //cur = k * n * n + i + j * n + j;
	  cur = k * n * n + diag_to_row_col(i, j, n);
	  if (A[cur] == 0.0) {
	    printf("searching> %g %d\n", A[cur], cur);
	    prev      = cur;
	    searching = false;
	    break;
	  }
	}
	/* move the next non-zero such that it is contiguous with
	   those previously seen */	
	swapping = true;
	for (j++; j < n && swapping; j++) {
	  //cur = k * n * n + i + j * n + j;
	  cur = k * n * n + diag_to_row_col(i, j, n);	
	  if (A[cur] != 0.0) {
	    printf(" swapping> %g %d -> %d\n", A[cur], cur, prev);
	    A[prev]  = A[cur];
	    A[cur]   = 0.0;
	    swapping = false;
	    prev++;
	  } else {
	    printf("%g %d\n", A[cur], cur);
	  }
	}
	printf("\n\nj = %d; n = %d\n\n", j, n);
      }
      printf("\n");
      print_diag(A, n);
      printf("\n");
    }
    break;
  }
}

int main(int argc, char *argv[])
{
  int n, seed;
  double *A;
  
  if (argc < 2) {
    fprintf(stderr, "Usage: matmul <n> [seed]\n");
    exit(1);
  }
  
  n    = atoi(argv[1]);
  seed = argc > 2 ? atoi(argv[2]) : 0;
  A    = (double*) malloc(n * n * n * sizeof(double));
  
  random_seed(seed);
  zero(A, n);
  init(A, n, 10);
  
  printf("A^[%d] (seed=%d):\n\n", n, seed);
  print(A, n);
  printf("diag(A^[%d]) (seed=%d):\n\n", n, seed);
  print_diag(A, n);
  printf("\n");
  
  compress_diag(A, n);
  
  printf("compress_diag(A^[%d]):\n\n", n, seed);
  print(A, n);
  printf("compress_diag(diag(A^[%d])) (seed=%d):\n\n", n, seed);
  print_diag(A, n);
  printf("\n");
  
  free(A);
  
  return 0;
}
