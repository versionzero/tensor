
#ifndef _FILE_H_
#define _FILE_H_

#include <stdio.h>

FILE* fopen_or_die(char const *filename, char const *mode);

char peek(FILE *file);

#endif

/*
  Local Variables:
  mode: C++
  End:
*/
