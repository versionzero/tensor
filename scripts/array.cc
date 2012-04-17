
#include <stdio.h>
#include <stdlib.h>

int
main(int argc, char *argv[])
{
  int i, j, k, n, m;
  
  n = (argc > 1) ? atoi(argv[1]) : 3;
  m = 1;
  
  printf("%%%%MatrixMarket tensor array real    general            \n");
  printf("%d %d %d\n", n, n, n);
  
  for (i = 0; i < n; ++i) {
    for (j = 0; j < n; ++j) {
      for (k = 0; k < n; ++k) {
	printf("%d ", m++);
      }
      printf("\n");
    }
  }
  
  return 0;
}
