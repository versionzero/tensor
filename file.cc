
#include "error.h"
#include "file.h"

FILE*
fopen_or_die(char const *filename, char const *mode)
{
  FILE *file;
  
  superfluous("fopen_or_die(filename='%s', mode='%s')\n", filename, mode);
  
  if (NULL == (file = fopen(filename, mode))) {
    die("Failed to open '%s' for %s.\n", filename, mode);
  }
  
  superfluous("fopen_or_die: file=0x%x\n", file);
  
  return file;  
}

char
peek(FILE *file)
{
  char c;
  
  if (feof(file)) {
    return EOF;
  }

  c = fgetc(file);
  ungetc(c, file);
  
  return c;
}
