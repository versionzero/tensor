
#pragma once
#ifndef _FORMAT_H_
#define _FORMAT_H_

/*--------------------------------------------------------------------*/

#include "storage.h"
#include "tensor.h"
#include "utility.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(storage);

/*--------------------------------------------------------------------*/

template<class T>
class tensor;

/*--------------------------------------------------------------------*/

END_NAMESPACE(storage);

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(format);

/*--------------------------------------------------------------------*/

const int precision = 32;

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(strategy);

typedef enum {
  coordinate = 0,
  max        = coordinate
} type;

END_NAMESPACE(strategy);

/*--------------------------------------------------------------------*/

template <class T>
class coordinate {

public:

  std::istream&
  read(std::istream& in, strategy::tensor<T>& data)  {
    
    T   v;
    int i, j, k;
    int n, nnz;
    
    /* determine the data's dimensionality */
    in >> n >> n >> n >> nnz;
    
    /* make sure we will not have to realloc during the read */
    data.initialize(n, nnz);
    
    /* read the data */
    while (nnz--) {
      in >> k >> i >> j >> v;
      data.set(k, j, i, v);
    }
    
    /* all done */
    return in;
    
  }

  std::ostream&
  write(std::ostream& out, strategy::tensor<T> const &data) const {
   
    T   v;
    int n, nnz;
    
    /* make sure we have some data */
    if (data.empty()) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }

    /* determine the data's dementionality */
    n = data.size();

    /* determine the number of non-zero entries */
    nnz = 0;
    for (int k = 0; k < n; ++k) {
      for (int i = 0; i < n; ++i) {
	for (int j = 0; j < n; ++j) {
	  if (0.0 != data.get(k, i, j)) {
	    nnz++;
	  }
	}
      }
    }
    
    /* print the dimensions so we can optionally use the output as
       input later */
    out << n << " " << n << " " << n << " " 
	<< nnz << std::endl;
    
    /* set the output format */
    out << std::setprecision(precision)
	<< std::scientific;
    
    /* print the data */
    for (int k = 0; k < n; ++k) {
      for (int i = 0; i < n; ++i) {
	for (int j = 0; j < n; ++j) {
	  if (0.0 != (v = data.get(k, i, j))) {
	    out << k << " " << i << " " << j << " " 
		<< v << std::endl;
	  }
	}
      }
    }
    
    /* all done */
    return out;
    
  }
  
};

/*--------------------------------------------------------------------*/

/* Compensate for the lack of templated typedefs.
   Usage: 
   sextended_karnaugh_map<float, acronyms::CSR> M;
   ** NOT WORKING because of template template parameters **
*/
#if 0
template<class T>
struct acronyms {
  typedef coordinate<T> COO;
  typedef compressed_sparse_row<T> CSR;
  typedef compressed_sparse_column<T> CSC;
  typedef block_sparse_row<T> BSR;
};
#endif

/*--------------------------------------------------------------------*/

END_NAMESPACE(format);

/*--------------------------------------------------------------------*/

#endif /* _FORMAT_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
