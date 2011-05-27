
#pragma once
#ifndef _STORAGE_H_
#define _STORAGE_H_

/*--------------------------------------------------------------------*/

#include "operation.h"
#include "utility.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <vector>

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(storage);

/*--------------------------------------------------------------------*/

const int precision = 10;

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(strategy);

typedef enum {
  canonical     = 0,
  crs_canonical = 1,
  naive_ekmr    = 2,
  rmo_ekmr      = 3,
  crs_ekmr      = 4,
  max           = crs_ekmr
} type;

END_NAMESPACE(strategy);

/*--------------------------------------------------------------------*/

template <class T>
class canonical {

public:

  typedef canonical<T> strategy_type;

public:

  canonical() : 
    data_(NULL), values_(NULL), n_(0) {
  }
  
  virtual
  ~canonical() {
    clear();
  }
  
  virtual
  int
  size() const {
    return n_;
  }
  
  virtual
  bool
  empty() const {
    return (size() == 0);
  }
  
  virtual
  void
  initialize(int n) {
    
    /* remove any existing data */
    clear();
    
    /* set the internal dimensions */
    n_ = n;
    
    /* reserve the memory we will need if we are going to be used
       without being read from a stream */
    reserve();
    
  }

  std::istream&
  read(std::istream &in)  {
    
    /* determine the data's dimensionality */
    int nnz = 0;
    in >> n_ >> n_ >> n_ >> nnz;
    
    /* make sure we will not have to realloc during the read */
    initialize(n_);
    
    /* read the data */
    int i, j, k;
    while (nnz--) {
      in >> k >> i >> j;
      in >> values_[k][i][j];
    }
    
    /* all done */
    return in;
    
  }
  
  std::ostream&
  write(std::ostream &out) const {
    
    /* make sure we have some data */
    if (empty()) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }
    
    /* determine the number of non-zero entries */
    int nnz = 0;
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  if (0.0 != values_[k][i][j]) {
	    nnz++;
	  }
	}
      }
    }
    
    /* print the dimensions and the number of non-zero entries so we
       can optionally use the output as input later */
    out << n_ << " " << n_ << " " << n_ << " "
	<< nnz << std::endl;
    
    /* set the output format */
    out << std::setprecision(precision)
	<< std::scientific;
    
    /* print the data */
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  if (0.0 != values_[k][i][j]) {
	    out << k << " " << i << " " << j << " " 
		<< values_[k][i][j] << std::endl;
	  }
	}
      }
    }
    
    /* all done */
    return out;
    
  }
  
  void
  add(strategy_type &result, strategy_type const &other) {

    /* reserve our output buffer */
    result.initialize(n_);
    
    /* perform the addition */
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  result.values_[k][i][j] = values_[k][i][j]
	    + other.values_[k][i][j];
	}
      }
    }

  }
  
  void
  subtract(strategy_type &result, strategy_type const &other) {

    /* reserve our output buffer */
    result.initialize(n_);

    /* perform the subtraction */
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  result.values_[k][i][j] = values_[k][i][j] 
	    - other.values_[k][i][j];
	}
      }
    }

  }

  void
  multiply(strategy_type &result, strategy_type const &other) {

    /* reserve and zero-out our output buffer */
    int const n = n_;
    result.initialize(n);
    
    /* make some shortcuts to the buffer names */
    T ***A = values_;
    T ***B = other.values_;
    T ***C = result.values_;
    
    /* perform the multiplication */
    for (int k = 0; k < n; ++k) {
      for (int i = 0; i < n; ++i) {
	for (int j = 0; j < n; ++j) {
	  for (int m = 0; m < n; ++m) {
	    C[k][i][j] += A[k][i][m] * B[k][m][j];
	  }
	}
      }
    }
    
  }
  
private:
  
  void
  clear ()  {
    if (values_) {
      for (int i = 0; i < n_; ++i ) {
	delete [] values_[i];
	values_[i] = NULL;
      }
      delete [] values_;
      values_ = NULL;
    }
    if (data_) {
      delete [] data_;
      values_ = NULL;
    }
    n_ = 0;
  }
  
  void
  reserve () {
    values_ = new T**[n_];
    for (int k = 0; k < n_; ++k) {
      values_[k] = new T*[n_];
    }
    T *p = data_ = new T[n_*n_*n_];
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	values_[k][i] = p;
	p += n_;
      }
    }
  }
  
  T   *data_, ***values_;
  int n_;
  
};

/*--------------------------------------------------------------------*/

template <class T>
class compressed_row_canonical {

public:

  typedef compressed_row_canonical<T> strategy_type;

public:

  compressed_row_canonical() : n_(0) {
  }
  
  virtual
  ~compressed_row_canonical() {
    clear();
  }
  
  virtual
  int
  size() const {
    return n_;
  }
  
  virtual
  bool
  empty() const {
    return (size() == 0);
  }
  
  virtual
  void
  initialize(int n) {
    
    /* remove any existing data */
    clear();
    
    /* set the internal dimensions */
    n_ = n;
    
    /* reserve the memory we will need if we are going to be used
       without being read from a stream */
    reserve();
    
  }

  std::istream&
  read(std::istream &in)  {
    
    /* determine the data's dimensionality */
    int nnz = 0;
    in >> n_ >> n_ >> n_ >> nnz;
    
    /* make sure we will not have to realloc during the read */
    initialize(n_);
    
    /* read the data */
    T   x;
    int i, j, jj, k;
    int current = -1;
    while (int m = 0; m < nnz; ++m) {
      in >> k >> i >> j >> x;
      if (i > current) {
	current = i;
	row_.push_back(m);
      }
      non_zero_.push_back(x);
      jj = j*n_ + k;
      column_.push_back(jj);
    }
    
    /* all done */
    return in;
    
  }
  
  std::ostream&
  write(std::ostream &out) const {
    
    /* make sure we have some data */
    if (empty()) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }

#if 0
    /* determine the number of non-zero entries */
    int nnz = 0;
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  if (0.0 != values_[k][i][j]) {
	    nnz++;
	  }
	}
      }
    }
    
    /* print the dimensions and the number of non-zero entries so we
       can optionally use the output as input later */
    out << n_ << " " << n_ << " " << n_ << " "
	<< nnz << std::endl;
    
    /* set the output format */
    out << std::setprecision(precision)
	<< std::scientific;
    
    /* print the data */
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  if (0.0 != values_[k][i][j]) {
	    out << k << " " << i << " " << j << " " 
		<< values_[k][i][j] << std::endl;
	  }
	}
      }
    }
#endif
    
    /* all done */
    return out;
    
  }
  
  void
  add(strategy_type &result, strategy_type const &other) {

    /* reserve our output buffer */
    result.initialize(n_);
    
#if 0
    /* perform the addition */
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  result.values_[k][i][j] = values_[k][i][j]
	    + other.values_[k][i][j];
	}
      }
    }
#endif

  }
  
  void
  subtract(strategy_type &result, strategy_type const &other) {

    /* reserve our output buffer */
    result.initialize(n_);

    /* perform the subtraction */
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  result.values_[k][i][j] = values_[k][i][j] 
	    - other.values_[k][i][j];
	}
      }
    }

  }

  void
  multiply(strategy_type &result, strategy_type const &other) {

    /* reserve and zero-out our output buffer */
    int const n = n_;
    result.initialize(n);
    
#if 0
    /* make some shortcuts to the buffer names */
    T ***A = values_;
    T ***B = other.values_;
    T ***C = result.values_;
    
    /* perform the multiplication */
    for (int k = 0; k < n; ++k) {
      for (int i = 0; i < n; ++i) {
	for (int j = 0; j < n; ++j) {
	  for (int m = 0; m < n; ++m) {
	    C[k][i][j] += A[k][i][m] * B[k][m][j];
	  }
	}
      }
    }
#endif
    
  }
  
private:
  
  void
  clear ()  {
    n_ = 0;
    non_zero_.clear();
    row_.clear();
    column_.clear();
  }
  
  void
  reserve () {
    int size = n_*n_;
    non_zero_.reserve(size);
    row_.reserve(n_);
    column_.reserve(size);
  }
  
  std::vector<T> non_zero_, row_, column_;
  int n_;
  
};

/*--------------------------------------------------------------------*/

template <class T>
class extended_karnaugh_map {
  
public:
  
  typedef extended_karnaugh_map<T> strategy_type;
  
public:
  
  extended_karnaugh_map() : data_(NULL), values_(NULL), n_(0) {
  }
  
  virtual
  ~extended_karnaugh_map() {
    clear();
  }
  
  virtual
  bool
  empty() const {
    return (0 == n_);
  }
  
  virtual
  int
  size() const {
    return n_;
  }
  
  virtual
  void
  initialize(int n) {
    
    /* remove any existing data */
    clear();
    
    /* set the internal dimenstions */
    n_ = n;
    
    /* reserve the memory we will need if we are going to be used
       without being read from a stream */
    reserve();
    
  }

  std::istream& 
  read(std::istream &in) {
    
    int nnz;                    /* number of non-zero entries */
    
    /* determine the data's dimensionality */
    in >> n_ >> n_ >> n_ >> nnz;
    
    /* make sure we will not have to realloc during the read */
    initialize(n_);
    
    /* read the data */
    int i, j, jj, k;
    while (nnz--) {
      in >> k >> i >> j;
      jj = j*n_ + k;
      in >> values_[i][jj];
    }
    
    /* all done */
    return in;
    
  }
  
  std::ostream& 
  write(std::ostream &out) const {
    
    /* make sure we have some data */
    if (empty()) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }

    /* determine the number of non-zero entries */
    int nnz = 0, nj = n_*n_;
    for (int i = 0; i < n_; ++i) {
      for (int j = 0; j < nj; ++j) {
	if (0.0 != values_[i][j]) {
	  nnz++;
	}
      }
    }
    
    /* print the dimensions so we can optionally use the output 
       as input later */
    out << n_ << " " << n_ << " " << n_ << " " 
	<< nnz << std::endl;
    
    /* set the output format */
    out << std::setprecision(precision)
	<< std::scientific;
    
    /* print the data */
    int jj;
    for (int k = 0; k < n_; ++k) {
      for (int i = 0; i < n_; ++i) {
	for (int j = 0; j < n_; ++j) {
	  jj = j*n_ + k;
	  if (0.0 != values_[i][jj]) {
	    out << k << " " << i << " " << j << " " 
		<< values_[i][jj] << std::endl;
	  }
	}
      }
    }
    
    /* all done */
    return out;
    
  }
  
protected:
  
  virtual
  void
  clear ()  {
    if (values_) {
      delete [] values_;
      values_ = NULL;
    }
    if (data_) {
      delete [] data_;
      values_ = NULL;
    }
    n_ = 0;
  }
  
  virtual
  void
  reserve () {
    data_   = new T[n_*n_*n_];
    values_ = new T*[n_];
    for (int i = 0; i < n_; ++i) {
      values_[i] = data_ + i*n_*n_;
    }
  }
  
  T   *data_, **values_;
  int n_;

};

/*--------------------------------------------------------------------*/

template <class T>
class naive_extended_karnaugh_map : public extended_karnaugh_map<T> {

public:

  typedef extended_karnaugh_map<T>       base_type;
  typedef naive_extended_karnaugh_map<T> strategy_type;

public:
  
  void
  add(strategy_type &result, strategy_type const &other) { 
    
    /* reserve our output buffer */
    int const n = base_type::n_;
    result.initialize(n);
    
    /* perform the addition */
    int const nj = n*n;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < nj; ++j) {
	result.values_[i][j] = base_type::values_[i][j] 
	  + other.values_[i][j];
      }
    }
    
  }
  
  void
  subtract(strategy_type &result, strategy_type const &other) {
    
    /* reserve our output buffer */
    int const n = base_type::n_;
    result.initialize(n);
    
    /* perform the addition */
    int const nj = n*n;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < nj; ++j) {
	result.values_[i][j] = base_type::values_[i][j] 
	  - other.values_[i][j];
      }
    }
    
  }

  void
  multiply(strategy_type &result, strategy_type const &other) {
    
    /* reserve and zero-out our output buffer */
    int const n = base_type::n_;
    result.initialize(n);

    /* make some shortcuts to the buffer names */
    T **A = base_type::values_;
    T **B = other.values_;
    T **C = result.values_;

    /* perform the multiplication */
    int const r = n*n;
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < r; ++j) {
	int jj = j%n;
	for (int m = 0, v = 0; m < n; ++m, v += n) {
	  C[i][j] += A[i][v+jj] * B[m][j];
	}
      }
    }
    
  }
  
};

/*--------------------------------------------------------------------*/

template <class T>
class rmo_extended_karnaugh_map : public naive_extended_karnaugh_map<T> {
  
public:
  
  typedef naive_extended_karnaugh_map<T> base_type;
  typedef rmo_extended_karnaugh_map<T>   strategy_type;
  
public:
  
  void
  multiply(strategy_type &result, strategy_type const &other) {
    
    /* reserve and zero-out our output buffer */
    int const n = base_type::n_;
    result.initialize(n);

    /* make some shortcuts to the buffer names */
    T **A = base_type::values_;
    T **B = other.values_;
    T **C = result.values_;
    
    /* perform the multiplication */
    for (int i = 0; i < n; ++i) {
      for (int j = 0, v = 0; j < n; ++j, v += n) {
	for (int m = 0, r = 0; m < n; ++m, r += n) {
	  for (int k = 0; k < n; ++k) {
	    C[i][k+r] += A[i][k+v] * B[j][k+r];
	  }
	}
      }
    }
    
  }
  
};

/*--------------------------------------------------------------------*/

template <class T>
class compressed_row_extended_karnaugh_map {
  
public:
  
  typedef compressed_row_extended_karnaugh_map<T> strategy_type;
  
public:
  
  compressed_row_extended_karnaugh_map() : 
    n_(0) {
  }
  
  virtual
  ~compressed_row_extended_karnaugh_map() {
    clear();
  }
  
  std::istream&
  read(std::istream &in) {
    
    int current;
    int i, j, jj, k, m;         /* indecies */
    int nnz;                    /* number of non-zero entries */
    T   x;
    
    /* determine the data's dimensionality */
    in >> n_ >> n_ >> n_ >> nnz;
    
    /* make sure we will not have to realloc during the read */
    initialize(n_);
    
    /* read the data */
    current = -1;
    while (m = 0; m < nnz; ++m) {
      in >> k >> i >> j >> x;
      if (i > current) {
	current = i;
	row_.push_back(m);
      }
      non_zero_.push_back(x);
      jj = j*n_ + k;
      column_.push_back(jj);
    }
    
    /* all done */
    return in;
    
  }

  std::ostream&
  write(std::ostream &out) const {
    
    T   zero;
    int i, j, km, nj;
    int p, r, np;

    /* make sure we have some data */
    if (empty()) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }
    
    /* print the dimensions and the number of non-zero entries so we
       can optionally use the output as input later */
    out << n_ << " " << n_ << " " << n_ << " "
	<< data_.size() << std::endl;
    
    /* print the data */
    i = 0;
    for (r = 1; r < row_.size(); ++r) {
      p  = row_[r-1];
      np = row_[r];
      for (; p < np; ++p) {
	i  = p;
	jj = column_[p];
	for (
	jj = ;
	k  = jj%n;
	j  = jj/n_;
	out << k << " " << i << " " << j << " " 
	    << data_[] << std::endl;
      }

#if 0
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
        if ( k < size && j == column_[k] ) {
          out << non_zero_[k++];
          if ( k > row_[m] ) {
            m++;
          }
        } else {
          out << zero;
        }
        out << " ";
      }
      if ( i != rows_ - 1 ) {
        out << std::endl;
      }
    }
#endif
    
    /* all done */
    return out;
    
  }
  
protected:
  
  virtual
  void
  clear()  {
    base_class::clear();
    non_zero_.clear();
    row_.clear();
    column_.clear();
  }
  
  virtual
  void
  reserve() {
    base_class::reserve();
    int size = n_*n_*n_;
    non_zero_.reserve(size);
    row_.reserve(n_);
    column_.reserve(size);
  }
  
  int n_;
  std::vector<T> data_, row_, column_;
  
};

/*--------------------------------------------------------------------*/

END_NAMESPACE(storage);

/*--------------------------------------------------------------------*/

void validate_strategy(int code);
storage::strategy::type extract_stategy(int code);
char const* strategy_to_string(int code);

/*--------------------------------------------------------------------*/

#endif /* _STORAGE_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
