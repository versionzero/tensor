
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
  int i, j;
     
  for (i = 0; i < n; i++) {
    for (j = 0; j < n; j++) {
      A[i * n + j] = 0.0;
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

int
row_col_to_diag(int i, int j, int n) {
  int d;
  
  d = i * n + j;
  if (i + j > n) {
    d = ++i * n + --j;
  }
  
  return d;
}

void
compress_diag(double *A, int n)
{
  int i, j, k, cur, prev;
  
  for (k = 0; k < n; k++) {
    for (i = 0; i < n; i++) {
      cur = prev = 0;
      for (j = 0; j < n*n; j += n) {
	printf("%g ", A[k * n * n + i + j]);
      }
      printf("\n");
    }
    printf("\n");
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
    printf("\n");
  }
}

int main(int argc, char *argv[])
{
  int n, seed;
  double *A;
  
  if (argc < 2) {
    fprintf(stderr, "Usage: matmul <n>\n");
    exit(1);
  }
  
  n    = atoi(argv[1]);
  seed = atoi(argv[2]);
  A    = (double*) malloc(n * n * n * sizeof(double));
  
  random_seed(seed);
  init(A, n, 10);
  
  printf("A^[%d] (seed=%d):\n\n", n, seed);
  print(A, n);
  printf("\n");
  
  compress_diag(A, n);
  
  printf("diag(A^[%d]):\n\n", n, seed);
  print(A, n);
  printf("\n");
  
  free(A);
  
  return 0;
}
