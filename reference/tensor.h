
#pragma once
#ifndef _TENSOR_H_
#define _TENSOR_H_

/*--------------------------------------------------------------------*/

#include "operation.h"
#include "storage.h"
#include "utility.h"

#include <iostream>
#include <vector>

/*--------------------------------------------------------------------*/

BEGIN_NAMESPACE(storage);

/*--------------------------------------------------------------------*/

template<class T, template <class> class Storage>
class tensor_impl;

/*--------------------------------------------------------------------*/

template<class T>
class tensor {

public:
 
  typedef tensor<T> tensor_type;

public:
  
  virtual 
  ~tensor() {
  }
  
  virtual std::istream& 
  read(std::istream &in) = 0;

  virtual std::ostream& 
  write(std::ostream &out) const = 0;
  
  virtual
  tensor_type*
  apply(operations::arithmetic::type operation, tensor_type const *other) = 0;
  
  static tensor<T>*
  create(storage::strategy::type strategy) {
    tensor<T> *p = NULL;
    switch (strategy) {
    case storage::strategy::canonical:
      p = new tensor_impl<T, storage::canonical>();
      break;
    case storage::strategy::crs_canonical:
      p = new tensor_impl<T, storage::compressed_row_canonical>();
      break;
    case storage::strategy::naive_ekmr:
      p = new tensor_impl<T, storage::naive_extended_karnaugh_map>();
      break;
    case storage::strategy::rmo_ekmr:
      p = new tensor_impl<T, storage::rmo_extended_karnaugh_map>();
      break;
    default:
      /* Do nothing? */
      break;
    }
    return p;
  }
  
};

/*--------------------------------------------------------------------*/

template<class T>
std::istream&
operator >>(std::istream &in, tensor<T> &t) {
  return t.read(in);
}

template<class T>
std::istream&
operator >>(std::istream &in, tensor<T> *t) {
  return t->read(in);
}

template<class T>
std::ostream&
operator <<(std::ostream &out, const tensor<T> &t) {
  return t.write(out);
}  

template<class T>
std::ostream&
operator <<(std::ostream &out, const tensor<T> *t) {
  return t->write(out);
}  

/*--------------------------------------------------------------------*/
  
template<class T, template <class> class Storage>
class tensor_impl : public tensor<T> {

public:

  typedef T                       numeric_type;
  typedef tensor<T>               tensor_type;
  typedef tensor_impl<T, Storage> tensor_impl_type;
  typedef tensor_impl_type const  const_tensor_impl_type;
  typedef Storage<T>              tensor_storage_type;
  
public:
  
  tensor_impl() {
  }
  
  virtual 
  ~tensor_impl() {
  }

  tensor_type*
  apply(operations::arithmetic::type operation, tensor_type const *p) {
    /* this is safe in that we assume tensor operations are only done
       on tensors of the same kind (for now) */
    const_tensor_impl_type *other 
      = dynamic_cast<const_tensor_impl_type*>(p);
    tensor_impl_type *result = new tensor_impl_type();
    switch(operation) {
    case operations::arithmetic::addition:
      strategy_.add(result->strategy_, other->strategy_);
      break;
    case operations::arithmetic::subtraction:
      strategy_.subtract(result->strategy_, other->strategy_);
      break;
    case operations::arithmetic::multiplication:
      strategy_.multiply(result->strategy_, other->strategy_);
      break;
    default:
      /* Do Nothing? */
      break;
    }    
    return result;
  }

  std::istream& 
  read(std::istream &in) {
    return strategy_.read(in);
  }
  
  std::ostream& 
  write(std::ostream &out) const {
    return strategy_.write(out);
  }
  
private:
  
  tensor_storage_type strategy_;
  
};

/*--------------------------------------------------------------------*/

END_NAMESPACE(storage);

/*--------------------------------------------------------------------*/

storage::tensor<double>* create_tensor(int code);
storage::tensor<double>* read_tensor(char const *fn, int s);

/*--------------------------------------------------------------------*/

#endif /* _TENSOR_H_ */

/*
  Local Variables:
  mode: C++
  End:
*/
