/*----------------------------------------------------------------------
  Headers
  ----------------------------------------------------------------------*/

#include "error.h"
#include "operation.h"
#include "storage.h"
#include "tensor.h"

#include <ctime>
#include <iostream>
#include <fstream>

/*----------------------------------------------------------------------
  Namespace inclusions
----------------------------------------------------------------------*/

using std::cerr;
using std::endl;
using std::ios_base;
using std::ifstream;

/*----------------------------------------------------------------------  
  Main Functions
----------------------------------------------------------------------*/

storage::tensor<double>*
create_tensor(int code)
{                               /* --- tensor factory */
  return storage::tensor<double>::create(extract_stategy(code));
}

/*--------------------------------------------------------------------*/

storage::tensor<double>*
read_tensor(char const *fn, int s)
{                               /* --- read a tensor from a stream */  
  storage::tensor<double> *p;   /* tensor object */
  ifstream                fin;  /* input  file */
  clock_t                 t;    /* timer for measurements */
  try { 
    t = clock();                /* start the timer */    
    fin.open(fn, ios_base::in); /* open the input file */
    p = create_tensor(s);       /* instantiate a tensor object using */    
    if (!p) { error(E_NOMEM); } /* the storage strategy defined at the */
    cerr << "Reading " << fn    /* command line (or the default) */
	 << " ... ";
    fin >> p;                   /* read tensor */
    cerr << std::fixed << "done [" 
	 << SEC_SINCE(t) << "s]." << endl;
  } catch (...) {    
    error(E_FOPEN, fn); }       /* print the error message */
  return p;                     /* return the tensor object */
}

/*----------------------------------------------------------------------
  Local Variables:
  mode: C++
  End:
----------------------------------------------------------------------*/
