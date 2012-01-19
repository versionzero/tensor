// needlessly impractical 

#ifndef _TENSOR_H_
#define _TENSOR_H_

#include <iostream>
#include <vector>

template<class T>
class basic_tensor {
  
public:
  
  typedef T                                value_type;
  typedef value_type&                      reference;
  typedef std::vector<value_type>          storage;
  typedef typename storage::iterator       iterator;
  typedef typename storage::const_iterator const_iterator;
  typedef typename storage::pointer        pointer;
  typedef typename storage::const_pointer  const_pointer;
  typedef size_t		           size_type;
  
private:
  
  size_type l_, m_, n_;
  storage   values_;
  
public:
  
  basic_tensor() : l_(0), m_(0), n_(0) {}
  virtual ~basic_tensor() {}
  
  virtual size_type nnz() const { return values_.size(); }
  
  virtual pointer       data()       { return values_.data(); }
  virtual const_pointer data() const { return values_.data(); }
  
  virtual size_type l() const { return l_; }
  virtual size_type m() const { return m_; }
  virtual size_type n() const { return n_; }
  
#if 0
  virtual reference       at(size_type i, size_type j, size_type k) = 0;
  virtual const reference at(size_type i, size_type j, size_type k) = 0;
#endif
  
  virtual void clear() { l_ = m_ = n_ = 0; values_.clear(); }
  
  iterator	 begin()       { return values_.begin(); }
  const_iterator begin() const { return values_.begin(); }
  iterator       end()         { return values_.end(); }
  const_iterator end() const   { return values_.end(); }
  
  virtual bool read(std::istream &in) = 0;
  virtual bool write(std::ostream &out) = 0;
  
  
};

#if 0
struct compressed {};
struct slice {};
struct ekmr {};
struct jds {};
#endif

template<class T>
class coordinate_tensor : public basic_tensor<T> {
  
public:
  
  typedef basic_tensor<T>                    base_type;
  typedef typename base_type::value_type     value_type;
  typedef typename base_type::reference	     reference;
  typedef typename base_type::storage	     storage;
  typedef typename base_type::iterator	     iterator;
  typedef typename base_type::const_iterator const_iterator;
  typedef typename base_type::pointer        pointer;
  typedef typename base_type::const_pointer  const_pointer;
  typedef typename base_type::size_type      size_type;

public:
  
  typedef struct {
    size_type x, y, z, i;
  } coordinate_tuple;
    
public:
  
  bool
  read(std::istream &in) {
  }
  
  bool
  write(std::ostream &out) {
  }
  
};

typedef basic_tensor<double> tensor;

#endif


/*
  Local Variables:
  mode: C++
  End:
*/
