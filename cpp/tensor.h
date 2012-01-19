// needlessly impractical 

#ifndef _TENSOR_H_
#define _TENSOR_H_

#include <iostream>
#include <vector>

template<class T>
class cubic_tensor {
  
public:
  
  typedef T                                value_type;
  typedef value_type&                      reference;
  typedef std::vector<value_type>          storage;
  typedef typename storage::iterator       iterator;
  typedef typename storage::const_iterator const_iterator;
  typedef typename storage::pointer        pointer;
  typedef typename storage::const_pointer  const_pointer;
  typedef size_t		           size_type;
  
protected:
  
  size_type n_;
  storage   values_;
  
public:
  
  basic_tensor() : n_(0) {}
  virtual ~basic_tensor() {}
  
  virtual size_type nnz() const { return values_.size(); }
  
  virtual pointer       values()       { return values_.data(); }
  virtual const_pointer values() const { return values_.data(); }
  
  virtual size_type size() const { return n_; }
  
#if 0
  virtual reference       at(size_type i, size_type j, size_type k) = 0;
  virtual const reference at(size_type i, size_type j, size_type k) = 0;
#endif
  
  virtual void clear() { n_ = 0; values_.clear(); }
  
  virtual std::istream& read(std::istream &in) = 0;
  virtual std::ostream& write(std::ostream &out) = 0;
  
  
};

#if 0
struct compressed {};
struct slice {};
struct ekmr {};
struct jds {};
#endif

template<class T>
class coordinate_tensor : public cubic_tensor<T> {
  
public:
  
  typedef basic_tensor<T>                    base_type;
  typedef typename base_type::value_type     value_type;
  typedef size_t                             index_type;
  typedef typename base_type::reference	     reference;
  typedef typename base_type::storage	     value_storage;
  typedef std::vector<index_type>	     index_storage;
  typedef typename base_type::iterator	     iterator;
  typedef typename base_type::const_iterator const_iterator;
  typedef typename base_type::pointer        pointer;
  typedef typename base_type::const_pointer  const_pointer;
  typedef typename base_type::size_type      size_type;

public:
  
  typedef struct {
  size_type x, y, z, i;
  } coordinate_type;

private:
  
  index_storage coordinates_;
  
public:
  
  std::istream&
  read(std::istream &in) {
    return in;
  }
  
  std::ostream&
  write(std::ostream &out) {
    return out;
  }
  
};

#endif


/*
  Local Variables:
  mode: C++
  End:
*/
