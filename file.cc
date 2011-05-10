
#include "error.h"
#include "file.h"

FILE*
fopen_or_die(char const *filename, char const *mode)
{
  FILE *file;
  
  if (NULL == (file = fopen(filename, mode))) {
    die("Failed to open '%s' for %s.\n", filename, mode);
  }
  
  return file;  
}

