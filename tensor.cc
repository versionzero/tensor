
#include "error.h"
#include "file.h"
#include "tensor.h"
#include "utility.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>

#define OPTION_MESSAGE(x,a,b)   (x ? a:b)
#define DEFAULT_ON_OR_OFF(x)    OPTION_MESSAGE(x, "on", "off")

#define DESCRIPTION "A tool for performing basic arithmetic on th-order tensors."
#define VERSION     "Version 0.01 (" __DATE__ "), " \
  "Copyright (C) 2011, and GPLv3'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

#define DEFAULT_VERBOSITY false

int  storage_strategy;
int  operation_code;
char *program_name;
bool verbose;

void
usage() 
{
  message("%s: %s\n\n", program_name, DESCRIPTION);
  message("%s\n\n", VERSION);
  message("Usage:\n");
  message("\t%s [options] <input1> <input2> [output]\n\n",
	  program_name);  
  message("Options:\n");
  message("\t-h\tthis screen\n");
  message("\t-v\ttoggle verbosity (default: %s)\n", 
	  DEFAULT_ON_OR_OFF(DEFAULT_VERBOSITY));
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

tensor_t*
timed_tensor_convert(tensor_t *source, strategy::type_t strategy, orientation::type_t orientation)
{
  clock_t  t;
  tensor_t *tensor;
  
  message("Converting from '%s' to '%s' ... ", 
	  strategy_to_string(source->strategy), 
	  strategy_to_string(strategy));
  t = clock();
  tensor = tensor_convert(source, strategy, orientation);
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
  
  /* just to be safe, set the tensors to null */
  t1 = t2 = tr = NULL;

  /* set the program's defaults */
  verbose = false;  
  
  /* extract any command-line options the user provided */
  opterr = 0; /* we will privide our own error messages */
  while (-1 != (c = getopt(argc, argv, "hv"))) {
    switch (c) {
    case 'h':
      usage();
      break;
    case 'v':
      verbose = !verbose;
      break;  
    case '?':
      if (isprint(optopt)) {
	die("Unknown option `-%c'.\n", optopt);
      } else {
	die("Unknown option character `\\x%x'.\n", optopt);
      }
      break;
    default:
      abort();
      break;
    }    
  }
  
  /* count the number of remaining arguments */
  if (argc-optind < 2) {
    usage();
  }
  
  offset = optind;
  name   = argv[offset++];
  t1     = timed_tensor_read(name);  
  debug("main: t1=0x%x\n", t1);
  
  name = argv[offset++];
  t2   = timed_tensor_read(name);  
  debug("main: t2=0x%x\n", t2);
  
  tr = timed_tensor_convert(t2, strategy::compressed, orientation::row);  
  debug("main: tr=0x%x\n", tr);
  
  if (offset == argc) {
    file = stdout;
    message("Writing stdout ... ");
  } else {
    name = argv[offset++];
    file = fopen_or_die(name, "w+");
    message("Writing %s ... ", name);
  }  
  t = clock();
  tensor_fwrite(file, tr);
  message("done [%lf]\n", SECONDS_SINCE(t));
  
  tensor_free(tr);
  tensor_free(t2);
  tensor_free(t1);
  
  return 0;
}

