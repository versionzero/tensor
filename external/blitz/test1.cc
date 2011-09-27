#include <blitz/array.h>
#include <stdio.h>
#include <stdlib.h>

BZ_USING_NAMESPACE(blitz)

void
die(char const *format, ...)
{
  va_list args;
  
  va_start(args, format);
  vprintf(format, args);
  va_end(args);
  
  exit(1);
}

FILE*
fopen_or_die(char const *filename, char const *mode)
{
  FILE *file;
  
  printf("fopen_or_die(filename='%s', mode='%s')\n", filename, mode);
  
  if (NULL == (file = fopen(filename, mode))) {
    die("Failed to open '%s' for %s.\n", filename, mode);
  }
  
  printf("fopen_or_die: file=0x%p\n", file);
  
  return file;  
}

typedef Array<double, 3> tensor_t;

tensor_t*
tensor_fread_matlab(FILE *file)
{
  int                  i, j, k;
  int                  l, m, n, q, nnz;
  int                  result;
  double               d;
  tensor_t             *tensor;
  
  printf("tensor_fread_matlab(0x%p)\n", file);
  
  /* Read matlab sparse tensor files of the form:
     
     ^X is a sparse tensor of size 10 x 10 x 10 with 96 nonzeros$
     ^	( 1, 1, 7)    0.3796$
     ^	( 1, 2, 5)    0.3191$
     ^	( 1, 3, 2)    0.9861$
	...
    
    Where ^ and $ are regular regex anchors (no pun intended).  Note
    that we intentionally skip the first word before starting to look
    for numeric characters.  We do this because matlab variables can
    have numbers in them, which would break the assumption that the
    first number to occur in the sentence was the first dimension.
  */
  
  if (4 != (result = fscanf(file, "%*[^ ]%*[^1234567890]%d%*[ x]%d%*[ x]%d with %d nonzeros\n", &l, &m, &n, &nnz))) {
    die("tensor_fread_matlab: failed to read tensor dimensions (%d).\n", result);
  }
  
  tensor  = new tensor_t(l, m, n);
  
  for (q = 0; q < nnz; ++q) {
    if (4 != (result = fscanf(file, "%*[ (]%u%*[ ,]%u%*[ ,]%u%*[)]%lg\n", &k, &i, &j, &d))) {
      die("tensor_fread_matlab: failed to process line %d of the input stream (%d).\n", q, result);
    }
    (*tensor)(i-1, j-1, k-1) = d;
  }
  
  return tensor;
}

tensor_t*
tensor_read(char const *filename)
{
  FILE                *file;
  tensor_t            *tensor;
  
  printf("tensor_read('%s')\n", filename);
  
  file   = fopen_or_die(filename, "r");
  tensor = tensor_fread_matlab(file);
  fclose(file);
  
  return tensor;
}

int main(int argc, char *argv[])
{
  Array<double, 3> *A;
  
  A = tensor_read(argv[1]);
  
  cout << "A = " << *A << endl;
  
  delete A;
  
  
  return 0;
}

