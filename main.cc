
#include "error.h"
#include "file.h"
#include "matrix.h"
#include "tensor.h"
#include "utility.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define OPTION_MESSAGE(x,a,b)   (x ? a:b)
#define DEFAULT_ON_OR_OFF(x)    OPTION_MESSAGE(x, "on", "off")

#define DESCRIPTION "A tool for performing basic arithmetic on th-order tensors."
#define VERSION     "Version 0.01 (" __DATE__ "), " \
  "Copyright (C) 2011, and GNU GPL'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

int  storage_strategy;
int  operation_code;
char *program_name;
bool verbose;

/* Determine the appropriate name to base the output file names on.
   Note that this command is destructive, so the input buffer *will*
   be modified. */
char*
basename(char *name)
{
  char *base, *dot;

  base = strrchr(name, '/');
  if (NULL != base && '/' == *base) {
    base++;
  } else {
    base = name;
  }
  if (NULL != (dot = strrchr(name, '.'))) {
    *dot = '\0';
  }
  
  return base;
}

void
usage() 
{
  message("%s: %s\n\n", program_name, DESCRIPTION);
  message("%s\n\n", VERSION);
  message("Usage:\n");
  message("\t%s [options] <input1> <input2> [output]\n\n",
	  program_name);  
  message("Options:\n");
#if 0
  message("\t-s#\t\ttensor storage strategy (default: %d)\n",
	  DEFAULT_TENSOR_STORAGE_STRATEGY);
  message("\t-d\t\ttoggle image debugging (default: %s)\n", 
	  DEFAULT_ON_OR_OFF(DEFAULT_IMAGE_DEBUGGING));
  message("\t-g#\t\tnumber of generations (default: %s)\n", 
	  DEFAULT_GENERATIONS);
  message("\t-m#\t\tspecies mutation factor (default: %d)\n",
	  DEFAULT_SPECIES_MUTATION_FACTOR);
  message("\t-p#\t\tpopulation size (default: %d)\n",
	  DEFAULT_POPULATION_SIZE);
  message("\t-q#\t\tJPEG quality (default: %d)\n",
	  DEFAULT_IMAGE_QUALITY);
  message("\t-r<name>\tresult quantization matrix population (default: none)\n");
  message("\t-s<name>\tseed   quantization matrix population (default: none)\n");  
  message("\t-v\t\ttoggle verbosity (default: %s)\n", 
	  DEFAULT_ON_OR_OFF(DEFAULT_IMAGE_DEBUGGING));
#endif
  exit(1);
}

tensor_t*
timed_tensor_read(char const *name)
{
  clock_t  t;
  tensor_t *tensor;
  
  message("Reading %s ... ", name);
  t = clock();
  tensor = tensor_read(name);
  message("done [%lf]\n", SECONDS_SINCE(t));

  return tensor;
}

int
main(int argc, char *argv[])
{
  int      c, offset;
  clock_t  t;
  char     *name;
  tensor_t *t1, *t2, *tr;
  FILE     *file;
  
  /* store the our name for future use */
  program_name = basename(argv[0]);
  
  /* extract any command-line options the user provided */
  while (-1 != (c = getopt(argc, argv, "c:dg:q:r:s:v"))) {
    switch (c) {
    case 'v':
      verbose = !verbose;
      break;
    default:
      die("Uknown option -%c.\n", c);
      break;
    }
  }
  
  /* count the number of remaining arguments */
  if (argc-optind < 2) {
    usage();
  }
  
  offset = optind;
  name = argv[offset++];
  t1 = timed_tensor_read(name);
  
  information("main: t1=0x%x\n", t1);
  
  name = argv[offset++];
  t2 = timed_tensor_read(name);
  
  information("main: t2=0x%x\n", t2);
  
  if (offset == argc) {
    file = stdout;
    message("Writing stdout ... ");
  } else {
    name = argv[offset++];
    file = fopen_or_die(name, "w+");
    message("Writing %s ... ", name);
  }  
  t = clock();
  tensor_fwrite(file, t1);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  tensor_delete(tr);
  tensor_delete(t2);
  tensor_delete(t1);
  
  return 0;
}

