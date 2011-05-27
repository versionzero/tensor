/*----------------------------------------------------------------------
  Headers
  ----------------------------------------------------------------------*/

#pragma implementation "storage.h"
#pragma implementation "tensor.h"

#include "error.h"
#include "storage.h"
#include "operation.h"
#include "storage.h"
#include "tensor.h"

#include <cassert>
#include <ctime>
#include <iostream>
#include <fstream>

/*----------------------------------------------------------------------
  Namespace inclusions
----------------------------------------------------------------------*/

using std::cerr;
using std::cin;
using std::cout;
using std::endl;
using std::ios_base;
using std::ifstream;
using std::ofstream;
using std::string;

/*----------------------------------------------------------------------
  Preprocessor Definitions
----------------------------------------------------------------------*/

#define NAME        "tensor"
#define DESCRIPTION "Perform basic arithmetic on th-order tensors."
#define VERSION     "Version 0.01 (2011.01.01), " \
  "Copyright (C) 2011, and GNU GPL'd, by Ben Burnett\n" \
  "This is free software; see the source for copying conditions.  There is NO\n" \
  "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE."

/*----------------------------------------------------------------------
  Type Definitions
----------------------------------------------------------------------*/

typedef storage::tensor<double> tensor_type;

/*----------------------------------------------------------------------  
  Global Variables
----------------------------------------------------------------------*/

char        *name;              /* program name for error messages */
static int  block    = 4;       /* cache block size */
static bool verbose  = true;    /* to be noisy, or not to be? */
static int  strategy = 0;       /* storage strategy for tensor */
static int  opcode   = 0;       /* which operation are we performing? */

/*----------------------------------------------------------------------  
  Main Functions
----------------------------------------------------------------------*/

static void 
help()
{                               /* --- print add. option information */
  cerr << "usage: " << name << " [options] <input1> <input2> [output]" << endl;
  cerr << DESCRIPTION << endl;
  cerr << VERSION << endl << endl;
  cerr << "-b      block size to use (default: " << block << ")" << endl;
  cerr << "-s      storage strategy:" << endl;
  cerr << "        0: Traditional (or Canonical)" << endl;
  cerr << "        1: Extended Karnaugh Map Representation" << endl;
  cerr << "-o      arithmetic operation:" << endl;
  cerr << "        0: Addition" << endl;
  cerr << "        1: Subtraction" << endl;
  cerr << "-v      toggle verbosity (default: " 
       << (verbose ? "on" : "off") << ")" << endl;
  exit(0);                      /* abort the program */
}  /* help() */

/*--------------------------------------------------------------------*/

int 
main(int argc, char **argv) 
{                               /* --- main function */
  int         i, k;             /* loop counters */
  int         n;                /* indecies */
  char        *s;               /* to traverse the options */
  char        **optarg = NULL;  /* option argument */
  char const  *fn1_in  = NULL;  /* name of first  tensor file */
  char const  *fn2_in  = NULL;  /* name of second tensor file */
  char const  *fn_out  = NULL;  /* name of output        file */
  ofstream    fout;             /* output file */
  ofstream    ferr;             /* error file (/dev/null on !verbose) */
  clock_t     t;                /* timer for measurements */
  tensor_type *t1, *t2;         /* the actual input data */
  tensor_type *result;          /* the output data */

  /* --- initialize streams to not sync c++ and c streams --- */
  std::ios::sync_with_stdio(false);

  /* --- initialize globals --- */
  name = argv[0];               /* get program name for error messages */
  
  /* --- print usage message --- */
  if (argc > 2) {               /* if arguments are given */
    cerr << name << " - " << DESCRIPTION << endl;
    cerr << VERSION << endl;    /* print a startup message */
  } else {                      /* if no arguments are given */
    help();                     /* print a usage message */
  }                             /* and abort the program */
  
  /* --- evaluate arguments --- */
  k = 0;
  for (i = 1; i < argc; ++i) {  /* traverse the arguments */
    s = argv[i];                /* get an option argument */
    if (optarg) { *optarg = s; optarg = NULL; continue; }
    if (('-' == *s) && *++s) {
                                /* -- if argument is an option */
      while(*s) {               /* traverse the options */
        switch(*s++) {          /* evaluate the options */
	case 'b': block     = (int) strtol(s, &s, 0); break;
	case 'h': help();                             break;
	case 'o': opcode    = (int) strtol(s, &s, 0); break;
	case 's': strategy  = (int) strtol(s, &s, 0); break;
	case 'v': verbose   = !verbose;               break;
	default : error(E_OPTION, *--s);              break;
        }                       /* set the option variables */
        if (optarg && *s) {     /* get an option argument */
	  *optarg = s; optarg = NULL; break; }
      } 
    } else {                    /* -- if argument is no option */
      switch(k++) {             /* evaluate non-options */
      case  0: fn1_in = s;      break;
      case  1: fn2_in = s;      break;
      case  2: fn_out = s;      break;
      default: error(E_ARGCNT); break;
      }                         /* note filenames */
    }
  }
  if (optarg) {
    error(E_OPTARG);            /* check the option argument */
  }

  /* --- if we are to be noisy, dump messages to stderr; otherwise,
     redirect stderr to the bit bin --- */
  if (!verbose) {
    ferr.open("/dev/null", ios_base::out); 
    cerr.rdbuf(ferr.rdbuf());   /* open '/dev/null' for writing */
  }
  
  /* --- validate arguments -- */
  n = block % 2;                /* the cach size must be a power of */
  if (n != 0 && n != 1) {       /* two. */
    error(E_BLOCKSZ);           
  }
  
  /* --- read the tensor data --- */
  cerr << "Using '" << strategy_to_string(strategy) 
       << "' representation." << endl;
  t1 = read_tensor(fn1_in, strategy);
  t2 = read_tensor(fn2_in, strategy);

  /* --- perform the arithmetic --- */
  t   = clock();                /* start the timer */
  cerr << "Performing " << operation_name(opcode) << " ... ";
  result = t1->apply(
       extract_operation(opcode), t2); /* perform arithmetic */
  cerr << std::fixed << "done [" 
       << SEC_SINCE(t) << "s]." << endl;

  /* --- open output file --- */
  if (fn_out && *fn_out) {
    fout.open(fn_out, ios_base::out);
    cout.rdbuf(fout.rdbuf());   /* open tensor file for output */
  } else {                      /* if no output file name is given, */
    fn_out = "<stdout>";        /* write to std. output */
  }
  
  /* --- write results --- */
  t   = clock();                /* start the timer */
  cerr << "Writing " << fn_out << " ... ";
  if (!cout) {
    error(E_FOPEN, fn_out);
  }
  cout << result;
  cerr << std::fixed << "done [" 
       << SEC_SINCE(t) << "s]." << endl;
  
  return 0;
}

/*----------------------------------------------------------------------
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
