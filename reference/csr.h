
#pragma once
#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <iostream>
#include <vector>

template <class T>
class array {

public:
  
  std::istream&
  read ( std::istream& in )  {
    
    /* remove any existing data */
    clear ();
    
    /* determine the data's dimensionality */
    in >> rows_ >> columns_;
    
    /* make sure we will not have to realloc during the read */
    reserve ();
    
    /* read the data */
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	in >> values_[i][j];
      }
    }
    
    /* all done */
    return in;
    
  }

  std::ostream&
  write ( std::ostream& out ) const {
    
    /* make sure we have some data */
    if ( 0 == rows_ || 0 == columns_ ) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }
    
    /* print the dimensions so we can optionally use the output 
       as input later */
    out << rows_ << " " << columns_ << std::endl;
    
    /* print the data */
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	out << values_[i][j] << " ";
      }
      out << std::endl;
    }
    
    /* all done */
    return out;
    
  }
  
  std::vector<T>
  multiply ( const std::vector<T>& v ) {

    /* define the output vector */
    std::vector<T> w;

#if 0
    /* make sure we have some data, return an empty vector on error */
    int size = non_zero_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to operate on!" << std::endl;
      return w;
    }

    /* pre-allocate the vector size */
    w.resize ( rows_ );

    /* do the multiplication */
    int index = 1;
    for ( int i = 0; i < size; ++i ) {
      if ( index != row_[i] ) {
	index = row_[i];
      }
      w[index] += non_zero_[i] * v[column_[i]];      
    }
#endif
    
    /* all done */
    return w;
    
  }
  
private:
  
  void
  clear ()  {
    if ( values_ ) {
      delete [] values_;
    }
    values_ = rows_ = columns_ = 0;
  }
  
  void
  reserve () {
    values_ = new T[rows_][columns_];
  }
  
  T   *values_;
  int rows_, columns_;
  
};

template <class T>
class coordinate {

public:
  
  std::istream&
  read ( std::istream& in )  {
    
    /* remove any existing data */
    clear ();
    
    /* determine the data's dimensions */
    in >> rows_ >> columns_;
    
    /* make sure we will not have to realloc during the read */
    reserve ();
    
    /* read the data */
    T x = T (), zero = T ();
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	in >> x;
	if ( x != zero ) {
	  non_zero_.push_back ( x );
	  row_.push_back ( i );
	  column_.push_back ( j );	    
	}
      }
    }
    
    /* all done */
    return in;
    
  }

  std::ostream&
  write ( std::ostream& out ) const {
    
    /* make sure we have some data */
    int size = non_zero_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }
    
    /* print the dimensions so we can optionally use the output 
       as input later */
    out << rows_ << " " << columns_ << std::endl;
    
    /* print the data */
    int k = 0;
    T zero = T ();
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	if ( k < size && i == row_[k] && j == column_[k] ) {
	  out << non_zero_[k++];
	} else {
	  out << zero;
	}
	out << " ";
      }
      if ( i != rows_ - 1 ) {
	out << std::endl;
      }
    }
    
    /* all done */
    return out;
    
  }
  
  std::vector<T>
  multiply ( const std::vector<T>& v ) {

    /* define the output vector */
    std::vector<T> w;

    /* make sure we have some data, return an empty vector on error */
    int size = non_zero_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to operate on!" << std::endl;
      return w;
    }

    /* pre-allocate the vector size */
    w.resize ( rows_ );

    /* do the multiplication */
    int index = 1;
    for ( int i = 0; i < size; ++i ) {
      if ( index != row_[i] ) {
	index = row_[i];
      }
      w[index] += non_zero_[i] * v[column_[i]];      
    }
    
    /* all done */
    return w;
    
  }
  
private:
  
  void
  clear ()  {
    rows_ = columns_ = 0;
    non_zero_.clear ();
    row_.clear ();
    column_.clear ();
  }
  
  void
  reserve () {
    int size = rows_ * columns_;
    non_zero_.reserve ( size );
    row_.reserve ( size );
    column_.reserve ( size );
  }
  
  std::vector<T> non_zero_, row_, column_;
  int rows_, columns_;
  
};

template <class T>
class compressed_sparse_row {

public:

  std::istream& 
  read ( std::istream& in ) {
    
    /* remove any existing data */
    clear ();
    
    /* determine the data's dimensions */
    in >> rows_ >> columns_;
    
    /* make sure we will not have to realloc during the read */
    reserve ();
    
    /* read the data */
    int current = -1, k = -1;
    T x = T (), zero = T ();
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	in >> x;
	if ( x != zero ) {
	  k++;
	  if ( i > current ) {
	    current = i;
	    row_.push_back ( k );
	  }
	  non_zero_.push_back ( x );	  
	  column_.push_back ( j );	  
	}
      }
    }
    row_.push_back ( non_zero_.size () );
    
    /* all done */
    return in;

  }

  std::ostream& 
  write ( std::ostream& out ) const {
    
    /* make sure we have some data */
    int size = non_zero_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }
    
    /* print the dimensions so we can optionally use the output 
       as input later */
    out << rows_ << " " << columns_ << std::endl;
    
    /* print the data */
    T zero = T ();
    int k = 0, m = 0;
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
    
    /* all done */
    return out;
    
  }

  std::vector<T>
  multiply ( const std::vector<T>& v ) {

    /* define the output vector */
    std::vector<T> w;

    /* make sure we have some data, return an empty vector on error */
    int size = row_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to operate on!" << std::endl;
      return w;
    }

    /* pre-allocate the vector size */
    w.resize ( rows_ );

    /* do the multiplication */
    for ( int i = 0; i < size; ++i ) {
      int next = row_[i+1];
      for ( int j = row_[i]; j < next; ++j ) {
	w[i] += non_zero_[j] * v[column_[j]];
      }
    }
    
    /* all done */
    return w;

  }

private:

  void
  clear ()  {
    rows_ = columns_ = 0;
    non_zero_.clear ();
    row_.clear ();
    column_.clear ();
  }
  
  void
  reserve () {
    int size = rows_ * columns_;
    non_zero_.reserve ( size );
    row_.reserve ( rows_ );
    column_.reserve ( size );
  }

  std::vector<T> non_zero_, row_, column_;
  int rows_, columns_;

};

template <class T>
class compressed_sparse_column {


public:

  std::istream& 
  read ( std::istream& in ) {
    
    /* remove any existing data */
    clear ();
    
    /* determine the data's dimensions */
    in >> rows_ >> columns_;
    
    /* make sure we will not have to realloc during the read */
    reserve ();
    
    /* read the data */
    int current = -1, k = -1;
    T x = T (), zero = T ();
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	in >> x;
	if ( x != zero ) {
	  k++;
	  if ( j > current ) {
	    current = j;
	    column_.push_back ( k );
	  }
	  non_zero_.push_back ( x );	  
	  row_.push_back ( i );
	}
      }
    }
    column_.push_back ( non_zero_.size () );
    
    /* all done */
    return in;

  }

  std::ostream& 
  write ( std::ostream& out ) const {
    
    /* make sure we have some data */
    int size = non_zero_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to print!" << std::endl;
      return out;
    }
    
    /* print the dimensions so we can optionally use the output 
       as input later */
    out << rows_ << " " << columns_ << std::endl;
    
    /* print the data */
    T zero = T ();
    int k = 0, m = 0;
    for ( int i = 0; i < rows_; ++i ) {
      for ( int j = 0; j < columns_; ++j ) {
	if ( k < size && i == row_[k] ) {
	  out << non_zero_[k++];
	  if ( k > column_[m] ) {
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
    
    /* all done */
    return out;
    
  }

  std::vector<T>
  multiply ( const std::vector<T>& v ) {

    /* define the output vector */
    std::vector<T> w;

    /* make sure we have some data, return an empty vector on error */
    int size = column_.size ();
    if ( size == 0 ) {
      std::cerr << "ERROR: no data to operate on!" << std::endl;
      return w;
    }

    /* pre-allocate the vector size */
    w.resize ( rows_ );

    /* do the multiplication */
    for ( int i = 0; i < size; ++i ) {
      int next = column_[i+1];
      for ( int j = column_[i]; j < next; ++j ) {
	w[i] += non_zero_[row_[j]] * v[j];
      }
    }
    
    /* all done */
    return w;

  }

private:

  void
  clear ()  {
    rows_ = columns_ = 0;
    non_zero_.clear ();
    row_.clear ();
    column_.clear ();
  }
  
  void
  reserve () {
    int size = rows_ * columns_;
    non_zero_.reserve ( size );
    row_.reserve ( size );
    column_.reserve ( columns_ );
  }

  std::vector<T> non_zero_, row_, column_;
  int rows_, columns_;

};

template <class T>
class block_sparse_row {
};

/* Compensate for the lack of templated typedefs.
   Usage: 
   smatrix<float, acronyms::CSR> M;
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

#endif /* _STORAGE_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
