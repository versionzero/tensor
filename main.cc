
#include "error.h"
#include "matrix.h"
//#include "tensor.h"
#include "utility.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define OPTION_MESSAGE(x,a,b)   (x ? a:b)
#define DEFAULT_ON_OR_OFF(x)    OPTION_MESSAGE(x, "on", "off")

#define DESCRIPTION "Perform basic arithmetic on th-order tensors."
#define VERSION     "Version 0.01 (2011.01.01), " \
  "Copyright (C) 2011, and GNU GPL'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

int  storage_strategy;
int  operation_code;
char *program_name;
bool verbose;

void
usage() 
{
  message("Usage:\n");
  message("\t%s [options] [options] <input1> <input2> [output]\n",
	  program_name);
  message("%s\n", DESCRIPTION);
  message("%s\n", VERSION);
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

#if 0
image_t*
timed_image_read(char *name, image_type_t type)
{
  clock_t t;
  image_t *image;
  
  message("Reading %s ... ", name);
  t     = clock();
  image = image_read(name, type);
  message("done [%lf]\n", SECONDS_SINCE(t));

  return image;
}

void
timed_image_write(char *name, image_t *image, jpeg_image_options_t *options)
{
  clock_t t;
  
  message("Writing %s ... ", name);
  t = clock();
  image_write(name, image, options);
  message("done [%lf]\n", SECONDS_SINCE(t));
}
#endif 

int
main(int argc, char *argv[])
{
  int                  c, offset;
  uint                 width, height;
  clock_t              t;
  char                 *name, *base;
  char                 *seed_population_name, 
                       *result_population_name;
  char                 filename[PATH_MAX];  

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

  /* count the number of remaining arguments, once we have parsed the
     options */
  if (argc-optind < 2) {
    usage();
  }  

#if 0
  name = argv[offset++];
  fprintf(stderr, "Reading %s ... ", name);
  t = clock();
  m2 = matrix_read(name);
  fprintf(stderr, "done [%lf]\n", SECONDS_SINCE(t));
  
  print_operation_description(op);
  t = clock();
  mr = perform_opertation(op, m1, m2);
  fprintf(stderr, "done [%lf]\n", SECONDS_SINCE(t));
  
  name = argv[offset++];
  fprintf(stderr, "Writing %s ... ", name);
  t = clock();
  matrix_write(name, mr, TRUE);
  fprintf(stderr, "done [%lf]\n", SECONDS_SINCE(t));
  
  matrix_delete(mr);
  matrix_delete(m2);
  matrix_delete(m1);
#endif
  
  return 0;
}

